# docker build -t name:mupdf - < Dockerfile

FROM ubuntu:latest

# RUN apt-get update && apt-get install -y \
#     software-properties-common
# RUN add-apt-repository universe
RUN apt-get update && apt-get install -y \
	pkg-config \
	clang \
    python3 \
    python3-pip \
    swig

RUN python3 -m pip install clang

# Note: escape the / in /project to prevent msys from converting it to a windows path before executing the command
#
# docker run -ti  --mount src=$( pwd ),target=\/project,type=bind  name:mupdf
# RUN python3 scripts/mupdfwrap.py
# -->
# docker run -ti  --mount src=$( pwd ),target=\/project,type=bind  name:mupdf /project/scripts/mupdfwrap.sh
