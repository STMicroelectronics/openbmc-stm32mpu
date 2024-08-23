FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PV = "0.4.12+${SRCPV}"
# need to have commit 74497e0fa5b69b15790d6697e1ebce13af842d4c "configure: treat all openssl-3.x releases the same"
SRCREV = "dd0a8c63c0311674cabfd781753e8374ff531409"
