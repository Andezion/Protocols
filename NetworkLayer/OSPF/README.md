## Пример

#### Пример ограничения multipath маршрутов для OSPF

![photo](photo/photo.png)

На маршрутизаторе ESR необходимо ограничить количество multipath маршрутов до 2-х.

В исходной схеме от маршрутизаторов Router 2, Router 3, Router 4  анонсируется маршрут до подсети 192.0.2.0/24 с одинаковым cost на маршрутизатор ESR. По умолчанию для OSPF включено 16 multipath маршрутов. В результате чего в таблице маршрутизации есть 3 multipath маршрута: 

```
ESR# show ip route
 Codes: C - connected, S - static, R - RIP derived,
        O - OSPF derived, IA - OSPF inter area route,
        E1 - OSPF external type 1 route, E2 - OSPF external type 2 route
        B - BGP derived, D - DHCP derived, K - kernel route, V - VRRP route
        i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
        * - FIB route
 
O     * 192.0.2.0/24       [150/30]          multipath                         [ospf1 02:33:15]  (1.1.1.1)
                                   via 203.0.113.14 on gi1/0/1.10 weight 1
                                   via 203.0.113.18 on gi1/0/1.20 weight 1
                                   via 203.0.113.22 on gi1/0/1.30 weight 1
```

Конфигурация маршрутизатора ESR с выключенным firewall:

```
ESR# show running-config
router ospf log-adjacency-changes
router ospf 1
  router-id 203.0.113.1
  area 0.0.0.0
    enable
  exit
  enable
exit
 
interface gigabitethernet 1/0/1.10
  ip firewall disable
  ip address 203.0.113.13/30
  ip ospf instance 1
  ip ospf
exit
interface gigabitethernet 1/0/1.20
  ip firewall disable
  ip address 203.0.113.17/30
  ip ospf instance 1
  ip ospf
exit
interface gigabitethernet 1/0/1.30
  ip firewall disable
  ip address 203.0.113.21/30
  ip ospf instance 1
  ip ospf
exit
```

Для решения поставленной задачи необходимо указать maximum-paths, равный 2, при настройке OSPF-процесса. Произведем необходимые изменения в конфигурации:

```
ESR# configure
ESR(config)# router ospf 1
ESR(config-ospf)# maximum-paths 2
ESR(config-ospf)# do commit
ESR(config-ospf)# do confirm
ESR(config-ospf)# end
```

В результате в таблице маршрутизации будут 2 multipath маршрута:

```
ESR# show ip route
 Codes: C - connected, S - static, R - RIP derived,
        O - OSPF derived, IA - OSPF inter area route,
        E1 - OSPF external type 1 route, E2 - OSPF external type 2 route
        B - BGP derived, D - DHCP derived, K - kernel route, V - VRRP route
        i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
        * - FIB route
 
O     * 192.0.2.0/24       [150/30]          multipath                         [ospf1 02:37:32]  (1.1.1.1)
                                   via 203.0.113.14 on gi1/0/1.10 weight 1
                                   via 203.0.113.18 on gi1/0/1.20 weight 1
```