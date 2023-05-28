FROM python:3.11-slim
ENV PYTHONUNBUFFERED True
WORKDIR /euclid

COPY ./requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY ./ .
CMD exec gunicorn --bind 0.0.0.0:$PORT --workers 1 --threads 8 --timeout 0 main:app
