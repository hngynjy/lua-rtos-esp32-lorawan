#
# Component Makefile
#

COMPONENT_SRCDIRS := ./gateway/src ./node/lmic ./common ./node/semtech/mac/region ./node/semtech/mac \
                     ./node/semtech/system/crypto ./node/semtech/system \
                     ./node/semtech/radio/sx1272 ./node/semtech/radio/sx1276 ./node/semtech/port \
                     ./node/semtech ./node
                     
COMPONENT_ADD_INCLUDEDIRS := ./gateway/inc ./node/lmic ./common ./node/semtech/mac/region ./node/semtech/mac \
							 ./node/semtech/include ./node/semtech/system/crypto ./node/semtech/system  \
							 ./node/semtech/radio ./node/semtech/radio/sx1272 ./node/semtech/radio/sx1276 \
							 ./node/semtech/port ./node