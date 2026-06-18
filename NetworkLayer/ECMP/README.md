ECMP (Equal-Cost Multi-Path)

ECMP - это метод маршрутизации в компьютерных сетях, позволяющий отправлять пакеты данных, идущие к одному и тому же получателю, по нескольким разным каналам с одинаковой стоимостью (метрикой). Это позволяет эффективно балансировать нагрузку и увеличивать общую пропускную способность сети, задействуя все доступные пути, а не только один главный маршрут

## Теория 

## Практика 

ECMP поддержан для протоколов маршрутизации OSPF и BGP.  По умолчанию для OSPF количество multipath маршрутов - 16, для BGP количество multipath маршрутов - 1.

> С версии ПО 1.34.6 поддержан настройка статического multipath маршрута. Максимальное количество next-hop для статического multipath маршрута - 10.

Для изменения количества next-hop multipath маршрутов для OSPF необходимо указать maximum-paths при настройке OSPF-процесса. Пример:

```
ESR# configure
ESR(config)# router ospf 1
ESR(config-ospf)# maximum-paths ?
  1-32  Number of paths
```

ECMP настраивается глобально для всех IBGP-процессов и EBGP-процессов. Для изменения количества next-hop multipath маршрутов необходимо использовать следующую команду:

```
ESR(config)# router bgp maximum-paths ?
  1-16  Number of paths
```

Если лимит next-hop multipath маршрута исчерпан, то остальные ECMP маршруты не отображаются в таблице маршрутизации FIB. Пример отображения multipath маршрутов в таблице маршрутизации для OSPF:

```
ESR# show ip route
 Codes: C - connected, S - static, R - RIP derived,
        O - OSPF derived, IA - OSPF inter area route,
        E1 - OSPF external type 1 route, E2 - OSPF external type 2 route
        B - BGP derived, D - DHCP derived, K - kernel route, V - VRRP route
        i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
        * - FIB route
 
O     * 192.0.2.1/32       [150/30]          multipath                         [ospf1 01:10:43]  (1.1.1.1)
                                   via 198.51.100.2 on gi1/0/1.2 weight 1
                                   via 198.51.100.6 on gi1/0/1.3 weight 1
                                   via 198.51.100.10 on gi1/0/1.4 weight 1
                                   via 198.51.100.14 on gi1/0/1.5 weight 1
                                   via 198.51.100.18 on gi1/0/1.6 weight 1
                                   via 198.51.100.22 on gi1/0/1.7 weight 1
```

