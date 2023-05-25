#!/bin/bash

# build image
cd deploy/proscenium &&
docker buildx build --platform linux/amd64 -t josiahdc/proscenium:latest . --push &&

# deploy image to gcp
gcloud run deploy proscenium --image josiahdc/proscenium:latest --region us-west2
