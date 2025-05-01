FROM ubuntu:22.04

ENV USER giaco
ENV MPI_SOURCE_CODE test_nodes.cpp

ENV DEBIAN_FRONTEND=noninteractive \
    HOME=/home/${USER}

RUN apt-get update -y && \
    apt-get install -y --no-install-recommends sudo apt-utils && \
    apt-get install -y --no-install-recommends openssh-server \
    gcc gfortran libopenmpi-dev openmpi-bin openmpi-common openmpi-doc binutils dnsutils && \
    apt-get clean && apt-get purge && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp*


RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
        sudo \
        apt-utils \
        openssh-server \
        gcc \
        g++ \
        gfortran \
        libopenmpi-dev \
        openmpi-bin \
        openmpi-common \
        openmpi-doc \
        binutils \
        dnsutils && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN mkdir /var/run/sshd
RUN echo 'root:giaco' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

RUN adduser --disabled-password --gecos "" ${USER} && \
    echo "${USER} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

ENV SSHDIR ${HOME}/.ssh/
RUN mkdir -p ${SSHDIR}

ADD ./.ssh/config ${SSHDIR}/config
ADD ./.ssh/id_rsa ${SSHDIR}/id_rsa
ADD ./.ssh/id_rsa.pub ${SSHDIR}/id_rsa.pub
ADD ./.ssh/id_rsa.pub ${SSHDIR}/authorized_keys

RUN chmod -R 600 ${SSHDIR}* && \
    chown -R ${USER}:${USER} ${SSHDIR}

RUN rm -rf ${HOME}/.openmpi && mkdir -p ${HOME}/.openmpi
ADD mca-params.conf ${HOME}/.openmpi/mca-params.conf
RUN chown -R ${USER}:${USER} ${HOME}/.openmpi

ADD ./fetch_hostnames.sh ${HOME}/fetch_hostnames.sh
# Add your files here
ADD ./mpi_hello_world.c ${HOME}/${MPI_SOURCE_CODE}
# Compile the program
ADD ./other/ ${HOME}
#RUN mpicc ${HOME}/${MPI_SOURCE_CODE} -o ${HOME}/mpiprogram
RUN mpic++ ${HOME}/test_nodes.cpp -o ${HOME}/DOCKERTEST

EXPOSE 22
CMD [ "/usr/sbin/sshd", "-D" ]
