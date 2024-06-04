#!/bin/bash

sed 's/“/"/g;s/”/"/g;s/ü/\x81/g;s/ö/\x92/g;s/ä/\x84/g' | sed "s/’/\\'/g"
