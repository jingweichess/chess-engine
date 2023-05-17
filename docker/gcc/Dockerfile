FROM gcc:12.1

WORKDIR /var/app

COPY ./commands.txt /var/app/bin

CMD cd /var/app && make && cd bin && ./jing-wei << commands.txt
