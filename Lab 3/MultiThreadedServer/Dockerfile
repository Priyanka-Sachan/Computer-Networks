# Get the latest base image for python
FROM python:latest

# Put files at the image '/server/' folder.
ADD ipc_server.py /server/
ADD config.properties /server/
ADD HelloWorld.html /server/
ADD HelloArt.html /server/
ADD Error403.html /server/
ADD Error404.html /server/
ADD Error503.html /server/
ADD images/image-1.jpg /server/images/
ADD images/image-2.jpeg /server/images/
ADD images/image-3.jpg /server/images/
ADD images/image-4.jpeg /server/images/
ADD images/image-5.png /server/images/

# '/server/' is base directory
WORKDIR /server/

# Expose port 1234 in the container
EXPOSE 1234

# execute the command
CMD [ "python3", "/server/ipc_server.py" ]