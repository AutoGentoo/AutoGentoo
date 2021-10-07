#########################################
## AutoGentoo build container dockerfile
## Author: Andrei Tumbar
## Date: August 28 2021
#########################################

FROM scratch
LABEL author="andreitumbar@gmail.com"

ARG STAGE3_URL
WORKDIR /

# Download and extract the stage3 into root
ADD ${STAGE3_URL} /
COPY . /autogentoo

RUN emerge-webrsync && emerge --sync
