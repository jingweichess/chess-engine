FROM gcc:13.2

WORKDIR /var/app

COPY ./commands.txt /var/app/bin

CMD cd /var/app && make && cd bin && ./jing-wei << commands.txt
