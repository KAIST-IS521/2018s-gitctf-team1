#!/bin/sh

cp -r ../Service .
cp -r ../hackttp .
docker build -t gitctf-team1 .
