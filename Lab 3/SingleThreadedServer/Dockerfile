# Get the latest base image for python
FROM python:latest

# Put files at the image '/server/' folder.
ADD ipc_server.py /server/
ADD HelloWorld.html /server/
ADD Error404.html /server/

# '/server/' is base directory
WORKDIR /server/

# Expose port 1234 in the container
EXPOSE 1234

# execute the command
CMD [ "python3", "/server/ipc_server.py" ]