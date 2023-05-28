#!/bin/bash

echo "what should we tag the container?"
read tag

# build image
cd deploy/proscenium &&
docker buildx build --platform linux/amd64 -t josiahdc/proscenium:${tag} . --push &&

# deploy image to gcp
gcloud run deploy proscenium-${tag} --image josiahdc/proscenium:${tag} --region us-west2
