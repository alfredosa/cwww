# Apache Kafka

Kafka is a Distributed Data streaming application, that allows for real-time data streaming.

It consists of:

1. **Broker**:
    - A broker is a core component of Apache Kafka and represents an individual server in the Kafka cluster. It acts as a message broker, storing and managing the published data.
    - Brokers are responsible for handling incoming messages, maintaining data persistence, and distributing messages across multiple partitions.
    - Each broker is assigned a unique identifier called a "broker ID," and it can host one or more Kafka topics, which are log-structured message streams.
    - Kafka brokers work together as a distributed system to provide scalability, fault tolerance, and high availability.
2. **Consumer:**
    - A consumer is an application or component that subscribes to Kafka topics to consume messages published by producers.
    - Consumers read messages from Kafka topics in a "pull" manner, where they actively request data from the broker.
    - Multiple consumer instances can exist for a single topic, forming consumer groups. Each consumer in the group receives a subset of the messages from different partitions, allowing for parallel processing.
    - Consumers can track their read position in each topic, referred to as the "offset," allowing them to resume reading from the last known position after a restart.
3. **Producer**:
    - A producer is an application or component that produces (publishes) data to Kafka topics.
    - Producers send messages to Kafka brokers, which then store and distribute them to the appropriate partitions within the topic.
    - Producers can send messages to a specific partition or let Kafka decide based on a partitioning strategy (e.g., round-robin, hash-based, or custom partitioners).
    - Messages produced by the producer are stored durably in Kafka and can be consumed by one or more consumers.

Docker compose:

```yaml
version: '2'
services:
  zookeeper-1:
    image: confluentinc/cp-zookeeper:latest
    environment:
      ZOOKEEPER_CLIENT_PORT: 2181
      ZOOKEEPER_TICK_TIME: 2000
    ports:
      - "22181:2181"

  zookeeper-2:
    image: confluentinc/cp-zookeeper:latest
    environment:
      ZOOKEEPER_CLIENT_PORT: 2181
      ZOOKEEPER_TICK_TIME: 2000
    ports:
      - "32181:2181"
  
  kafka-1:
    image: confluentinc/cp-kafka:latest
    depends_on:
      - zookeeper-1
      - zookeeper-2
    ports:
      - "29092:29092"
    environment:
      KAFKA_BROKER_ID: 1
      KAFKA_ZOOKEEPER_CONNECT: "zookeeper-1:2181,zookeeper-2:2181"
      KAFKA_ADVERTISED_LISTENERS: "PLAINTEXT://kafka-1:9092,PLAINTEXT_HOST://localhost:29092"
      KAFKA_LISTENER_SECURITY_PROTOCOL_MAP: "PLAINTEXT:PLAINTEXT,PLAINTEXT_HOST:PLAINTEXT"
      KAFKA_INTER_BROKER_LISTENER_NAME: "PLAINTEXT"
      KAFKA_OFFSETS_TOPIC_REPLICATION_FACTOR: 1

  producer-nasa:
    build: ./producers
    command: python kafka_nasa_producer.py
    depends_on:
      - kafka-1
      - kafka-2
    environment:
      NASA_API_KEY: ${NASA_API_KEY}

  consumer-s3:
    build: ./consumers
    command: python kafka_s3_consumer.py
    depends_on:
      - kafka-1
      - kafka-2
    environment:
      AWS_ACCESS_KEY_ID: ${AWS_ACCESS_KEY_ID}
      AWS_SECRET_ACCESS_KEY: ${AWS_SECRET_ACCESS_KEY}
      AWS_DEFAULT_REGION: eu-west-2
      S3_BUCKET: orion-prefect
```

zookeeper - will be our brokers

Consumer:

```python
import os
import boto3
from kafka import KafkaConsumer

# Kafka consumer configuration
consumer = KafkaConsumer(
    'nasa_data_topic',
    bootstrap_servers='localhost:29092',
    auto_offset_reset='earliest',
    enable_auto_commit=True,
    group_id='my-consumer-group'
)

# AWS credentials and S3 bucket details
aws_access_key_id = os.environ['AWS_ACCESS_KEY_ID']
aws_secret_access_key = os.environ['AWS_SECRET_ACCESS_KEY']
aws_region = os.environ['AWS_DEFAULT_REGION']
s3_bucket = os.environ['S3_BUCKET']

# Initialize S3 client
s3 = boto3.client(
    's3',
    aws_access_key_id=aws_access_key_id,
    aws_secret_access_key=aws_secret_access_key,
    region_name=aws_region
)

def upload_to_s3(data):
    # Assuming the data is in JSON format, you may need to modify this based on your data structure
    file_key = f"{data['date']}.json"

    # Upload data to S3 bucket
    s3.put_object(
        Bucket=s3_bucket,
        Key=file_key,
        Body=str(data)
    )

if __name__ == '__main__':
    for message in consumer:
        nasa_data = message.value
        upload_to_s3(nasa_data)
```

**PRODUCER**:

```python
import requests
import os
from kafka import KafkaProducer

producer = KafkaProducer(bootstrap_servers='localhost:29092')
topic = 'nasa_data_topic'

# Get the NASA API key from the environment variable
nasa_api_key = os.environ['NASA_API_KEY']

# NASA API URL
api_url = f'https://api.nasa.gov/planetary/apod?api_key={nasa_api_key}'

def fetch_nasa_data():
    response = requests.get(api_url)
    if response.status_code == 200:
        data = response.json()
        return data
    else:
        return None

def send_nasa_data_to_kafka(data):
    producer.send(topic, value=data)

if __name__ == '__main__':
    nasa_data = fetch_nasa_data()
    if nasa_data:
        send_nasa_data_to_kafka(nasa_data)
```