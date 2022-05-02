# Get the latest base image for python
FROM python:latest

# Put files at the image '/server/' folder.
ADD udp_server.py /server/
ADD sample.txt /server/

# '/server/' is base directory
WORKDIR /server/

# Expose port 1234 in the container
EXPOSE 1234/udp

# execute the command
CMD [ "python3", "/server/udp_server.py" ]