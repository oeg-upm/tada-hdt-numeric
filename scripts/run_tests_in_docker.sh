docker image build -t tada-hdt-numeric:latest .
docker container run  --interactive --env CODECOV_TOKEN=$CODECOV_TOKEN --tty --rm --name tada-hdt-numeric tada-hdt-numeric:latest
