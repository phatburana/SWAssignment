import os
from dotenv import load_dotenv

basedir = os.path.abspath(os.path.dirname(__file__))
load_dotenv(os.path.join(basedir, '.env'))


class Config(object):
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'you-will-never-guess'
    SQLALCHEMY_DATABASE_URI = os.environ.get('DATABASE_URL') or \
            "postgresql://postgres:0894565295@localhost:5432/ictes"
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    API_KEY = 'ictes710-MbedDisco'
