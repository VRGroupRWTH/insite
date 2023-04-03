FROM python:3.10

RUN pip install tvb-library
RUN pip install tvb-framework 
RUN pip install orjson numpy 

COPY ./websocketmanager.py /insite/websocketmanager.py
COPY ./monitor_wrapper.py /insite/monitor_wrapper.py
