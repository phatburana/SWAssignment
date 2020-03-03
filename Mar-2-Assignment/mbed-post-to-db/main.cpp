#include "mbed.h"
#include "https_request.h"
#include "network-helper.h"
#include "ssl_ca_pem.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"

Serial pc(USBTX, USBRX);
float sensor_value = 0;

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%lu bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

int main() {
    nsapi_error_t r;
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();

    pc.printf("Starting\n");
    NetworkInterface* network = connect_to_default_network_interface();

    TLSSocket* socket = new TLSSocket();

    if ((r = socket->open(network)) != NSAPI_ERROR_OK) {
        printf("TLS socket open failed (%d)\n", r);
        return 1;
    }

    if ((r = socket->set_root_ca_cert(SSL_CA_PEM)) != NSAPI_ERROR_OK) {
        printf("TLS socket set_root_ca_cert failed (%d)\n", r);
        return 1;
    }
    if ((r = socket->connect("https://nameless-hamlet-67722.herokuapp.com", 443)) != NSAPI_ERROR_OK) {
        printf("TLS socket connect failed (%d)\n", r);
        return 1;
    }


    while(1) {
        // POST request
        {
            HttpsRequest* post_temp = new HttpsRequest(socket, HTTP_POST, "https://nameless-hamlet-67722.herokuapp.com/update");
            post_temp->set_header("Content-Type", "application/json");
            char body[100];
            sensor_value = BSP_TSENSOR_ReadTemp();
            sprintf(body, "{\"data\":{\"temperature\": %.2f}}", sensor_value);
            HttpResponse* post_res = post_temp->send(body, strlen(body));
            if (!post_res) {
                printf("HttpRequest failed (error code %d)\n", post_temp->get_error());
                NVIC_SystemReset();
                return 1;
            }
            printf("\n----- HTTPS POST response -----\n");
            dump_response(post_res);
            delete post_temp;
        }
        {
            HttpsRequest* post_humid = new HttpsRequest(socket, HTTP_POST, "https://nameless-hamlet-67722.herokuapp.com/humidity");
            post_humid->set_header("Content-Type", "application/json");
            char body[100];
            sensor_value = BSP_HSENSOR_ReadHumidity();
            sprintf(body, "{\"data\":{\"humidity\": %.2f}}", sensor_value);
            HttpResponse* post_res = post_humid->send(body, strlen(body));
            if (!post_res) {
                printf("HttpRequest failed (error code %d)\n", post_humid->get_error());
                NVIC_SystemReset();
                return 1;
            }
            printf("\n----- HTTPS POST response -----\n");
            dump_response(post_res);
            delete post_humid;
        }
        wait(10);

    }

        socket->close();
        delete socket;


}

