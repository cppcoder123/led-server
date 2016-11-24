v 20130925 2
C 47000 43800 1 0 0 connector2-1.sym
{
T 47200 44800 5 10 0 0 0 0 1
device=CONNECTOR_2
T 47000 44600 5 10 1 1 0 0 1
refdes=CONN_POW_1
T 47000 43800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 47000 41800 1 0 0 connector2-1.sym
{
T 47200 42800 5 10 0 0 0 0 1
device=CONNECTOR_2
T 47000 42600 5 10 1 1 0 0 1
refdes=CONN_POW_3
T 47000 41800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 47000 40800 1 0 0 connector2-1.sym
{
T 47200 41800 5 10 0 0 0 0 1
device=CONNECTOR_2
T 47000 41600 5 10 1 1 0 0 1
refdes=CONN_POW_4
T 47000 40800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 47000 42800 1 0 0 connector2-1.sym
{
T 47200 43800 5 10 0 0 0 0 1
device=CONNECTOR_2
T 47000 43600 5 10 1 1 0 0 1
refdes=CONN_POW_2
T 47000 42800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 36100 38000 0 0 0 title-bordered-A2.sym
N 48700 44300 49500 44300 4
N 48700 44000 50000 44000 4
N 48700 43300 49500 43300 4
N 48700 43000 50000 43000 4
N 48700 42300 49500 42300 4
N 48700 42000 50000 42000 4
N 48700 41300 49500 41300 4
N 48700 41000 50000 41000 4
N 50000 40600 50000 44000 4
C 43700 48500 1 0 0 ATmega8-1.sym
{
T 43800 53300 5 10 0 0 0 0 1
footprint=DIP28N
T 46000 53100 5 10 1 1 0 6 1
refdes=U1
T 43800 54100 5 10 0 0 0 0 1
device=ATmega8
}
C 49900 40300 1 0 0 gnd-1.sym
C 49300 44500 1 0 0 vcc-1.sym
N 43700 49500 41900 49500 4
N 41700 49700 43700 49700 4
C 39800 53400 1 0 0 resistor-2.sym
{
T 40200 53750 5 10 0 0 0 0 1
device=RESISTOR
T 40000 53700 5 10 1 1 0 0 1
refdes=R0
T 39800 53400 5 10 0 0 0 0 1
footprint=R025
}
C 41000 53400 1 0 0 led-2.sym
{
T 41800 53700 5 10 1 1 0 0 1
refdes=D0
T 41100 54000 5 10 0 0 0 0 1
device=LED
T 41000 53400 5 10 0 0 0 0 1
footprint=LED5
}
N 41000 53500 40700 53500 4
N 39800 53500 39400 53500 4
C 40300 52900 1 0 0 resistor-2.sym
{
T 40700 53250 5 10 0 0 0 0 1
device=RESISTOR
T 40500 53200 5 10 1 1 0 0 1
refdes=R1
T 40300 52900 5 10 0 0 0 0 1
footprint=R025
}
C 41500 52900 1 0 0 led-2.sym
{
T 41600 53500 5 10 0 0 0 0 1
device=LED
T 42300 53200 5 10 1 1 0 0 1
refdes=D1
T 41500 52900 5 10 0 0 0 0 1
footprint=LED5
}
N 41500 53000 41200 53000 4
N 39400 53000 40300 53000 4
C 39800 52400 1 0 0 resistor-2.sym
{
T 40200 52750 5 10 0 0 0 0 1
device=RESISTOR
T 40000 52700 5 10 1 1 0 0 1
refdes=R2
T 39800 52400 5 10 0 0 0 0 1
footprint=R025
}
C 41000 52400 1 0 0 led-2.sym
{
T 41100 53000 5 10 0 0 0 0 1
device=LED
T 41800 52700 5 10 1 1 0 0 1
refdes=D2
T 41000 52400 5 10 0 0 0 0 1
footprint=LED5
}
N 41000 52500 40700 52500 4
N 39800 52500 39400 52500 4
N 39400 52300 39400 53500 4
C 39300 52000 1 0 0 gnd-1.sym
N 41900 52500 42800 52500 4
N 42400 53000 42600 53000 4
N 42800 52500 42800 52400 4
N 42800 52400 43700 52400 4
N 42600 53000 42600 52600 4
N 42600 52600 43700 52600 4
N 41900 53500 42800 53500 4
N 42800 53500 42800 52800 4
N 42800 52800 43700 52800 4
C 42600 47900 1 0 0 crystal-1.sym
{
T 42800 48400 5 10 0 0 0 0 1
device=CRYSTAL
T 42800 48200 5 10 1 1 0 0 1
refdes=U_CRYS
T 42800 48600 5 10 0 0 0 0 1
symversion=0.1
T 42600 47900 5 10 0 0 0 0 1
footprint=CRYSTAL 300
}
N 43700 48900 42400 48900 4
N 42400 47400 42400 48900 4
N 43700 48700 43500 48700 4
N 43500 47400 43500 48700 4
N 42600 48000 42400 48000 4
N 43300 48000 43500 48000 4
C 42600 46500 1 90 0 capacitor-1.sym
{
T 41900 46700 5 10 0 0 90 0 1
device=CAPACITOR
T 42100 46700 5 10 1 1 90 0 1
refdes=C1
T 41700 46700 5 10 0 0 90 0 1
symversion=0.1
T 42600 46500 5 10 0 0 0 0 1
footprint=CK05_type_Capacitor
}
C 43700 46500 1 90 0 capacitor-1.sym
{
T 43000 46700 5 10 0 0 90 0 1
device=CAPACITOR
T 43200 46700 5 10 1 1 90 0 1
refdes=C2
T 42800 46700 5 10 0 0 90 0 1
symversion=0.1
T 43700 46500 5 10 0 0 0 0 1
footprint=CK05_type_Capacitor
}
N 42400 46500 42400 46300 4
N 43500 46500 43500 46300 4
N 42400 46300 45300 46300 4
C 42900 45800 1 0 0 gnd-1.sym
N 43000 46100 43000 46300 4
C 50000 52800 1 0 1 connector2-1.sym
{
T 49800 53800 5 10 0 0 0 6 1
device=CONNECTOR_2
T 50000 53600 5 10 1 1 0 6 1
refdes=CONN_HL_VCC
T 50000 52800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 50000 51800 1 0 1 connector2-1.sym
{
T 49800 52800 5 10 0 0 0 6 1
device=CONNECTOR_2
T 50000 52600 5 10 1 1 0 6 1
refdes=CONN_HL_DATA
T 50000 51800 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 50000 50400 1 0 1 connector2-1.sym
{
T 49800 51400 5 10 0 0 0 6 1
device=CONNECTOR_2
T 50000 51200 5 10 1 1 0 6 1
refdes=CONN_HR_VCC
T 50000 50400 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 50000 49300 1 0 1 connector2-1.sym
{
T 49800 50300 5 10 0 0 0 6 1
device=CONNECTOR_2
T 50000 50100 5 10 1 1 0 6 1
refdes=CONN_HR_DATA
T 50000 49300 5 10 0 0 0 0 1
footprint=MTA_156 2
}
N 46300 51800 47000 51800 4
N 47000 51800 47000 53000 4
N 47000 53000 48300 53000 4
N 46300 51600 47200 51600 4
N 47200 51600 47200 52300 4
N 47200 52300 48300 52300 4
N 46300 51400 47400 51400 4
N 47400 51400 47400 52000 4
N 47400 52000 48300 52000 4
N 46300 51200 47500 51200 4
N 47300 51000 46300 51000 4
N 46300 50800 47100 50800 4
C 50000 48000 1 0 1 connector2-1.sym
{
T 49800 49000 5 10 0 0 0 6 1
device=CONNECTOR_2
T 50000 48800 5 10 1 1 0 6 1
refdes=CONN_H_GND
T 50000 48000 5 10 0 0 0 0 1
footprint=MTA_156 2
}
C 47900 53500 1 0 0 vcc-1.sym
C 47500 51200 1 0 0 vcc-1.sym
C 48000 47700 1 0 0 gnd-1.sym
N 48300 48200 48100 48200 4
N 48100 48000 48100 48500 4
T 50200 53300 9 10 1 0 0 0 1
VCC
T 50100 50900 9 10 1 0 0 0 1
VCC
T 50200 48100 9 10 1 0 0 0 1
GND
N 49500 44500 49500 41300 4
C 37500 47200 1 0 0 connector10-1.sym
{
T 39400 50200 5 10 0 0 0 0 1
device=CONNECTOR_10
T 37600 50400 5 10 1 1 0 0 1
refdes=CONN_ISP
T 37500 47200 5 10 0 0 0 0 1
footprint=0.1_inch_10pin
}
N 46300 50600 46900 50600 4
N 46900 45000 46900 50600 4
N 41700 48300 41700 49100 4
N 41900 47700 41900 49300 4
N 39200 48300 41700 48300 4
N 39200 48900 41500 48900 4
N 41500 48900 41500 45000 4
N 41500 45000 46900 45000 4
C 39500 50300 1 0 0 vcc-1.sym
N 39200 49800 39700 49800 4
N 39700 49800 39700 50300 4
N 39200 50100 41900 50100 4
C 39300 45100 1 0 0 gnd-1.sym
N 39200 47400 39400 47400 4
N 39400 47400 39400 45400 4
N 48100 53500 48100 53300 4
N 48100 53300 48300 53300 4
N 47700 51200 47700 50900 4
N 47700 50900 48300 50900 4
N 48300 48500 48100 48500 4
T 50500 48600 9 10 1 0 0 0 1
Share this connector between 2 ht1632c
C 46400 50100 1 0 0 vcc-1.sym
N 46300 49500 46600 49500 4
N 46600 49500 46600 50100 4
N 47500 51200 47500 50600 4
N 47500 50600 48300 50600 4
N 47300 51000 47300 49800 4
N 47300 49800 48300 49800 4
N 47100 50800 47100 49500 4
N 47100 49500 48300 49500 4
C 45500 46500 1 90 0 capacitor-1.sym
{
T 44800 46700 5 10 0 0 90 0 1
device=CAPACITOR
T 45000 46700 5 10 1 1 90 0 1
refdes=C_POW
T 44600 46700 5 10 0 0 90 0 1
symversion=0.1
T 45500 46500 5 10 0 0 0 0 1
footprint=CK05_type_Capacitor
}
N 45300 46300 45300 46500 4
C 45100 47600 1 0 0 vcc-1.sym
N 45300 47600 45300 47400 4
N 41900 49500 41900 50100 4
N 43700 49300 41900 49300 4
N 43700 49100 41700 49100 4
N 41700 49700 41700 49500 4
N 41700 49500 39200 49500 4
C 40400 47600 1 0 0 resistor-2.sym
{
T 40800 47950 5 10 0 0 0 0 1
device=RESISTOR
T 40600 47900 5 10 1 1 0 0 1
refdes=R6
T 40400 47600 5 10 0 0 0 0 1
footprint=R025
}
C 40300 46600 1 90 0 resistor-2.sym
{
T 39950 47000 5 10 0 0 90 0 1
device=RESISTOR
T 40000 46800 5 10 1 1 90 0 1
refdes=R7
T 40300 46600 5 10 0 0 90 0 1
footprint=R025
}
N 40200 46600 40200 46200 4
N 40200 46200 39400 46200 4
N 40400 47700 39200 47700 4
N 40200 47500 40200 47700 4
N 41900 47700 41300 47700 4
C 37000 51000 1 0 0 connector2-1.sym
{
T 37200 52000 5 10 0 0 0 0 1
device=CONNECTOR_2
T 37000 51800 5 10 1 1 0 0 1
refdes=C_RLY
T 37000 51000 5 10 0 0 0 0 1
footprint=MTA_156 2
}
N 38700 51200 38700 51500 4
N 38700 51400 43700 51400 4
T 38500 42600 9 16 1 0 0 0 5
Attention: Add a jumper near R6 to shunt
it. MISO level should not be divided while 
programming. 
On the other hand voltage should be divided 
for communication with openwrt board
