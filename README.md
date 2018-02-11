# alarm-monitor-server
Arduino based, Prometheus compatible metrics server for fire alarm panel.

The project creates a Prometheus compatible metrics endpoint to allow timeseries data to be collected about the status of a fire alarm panel.

For more info see:
- Prometheus: https://prometheus.io/
- Alertmanager: https://prometheus.io/docs/alerting/alertmanager/
- Grafana: https://grafana.com/

To make use of this, open the AlarmMonitorServer.ino file with arduino IDE.

Edit the static IP address, choosing an available IP address suitable for your local network that is outside of DHCP allocated address space.

The hardware required depends on the alarm box itself. My alarm has relays for fault and alarm condition with NO/NC contacts available.
The resistor divider of 10k and 4.7k provides a roughly 15v range mapped onto the 0-5v range of analog input zero, so is suitable for monitoring a 12v SLA battery.

Digital pins 6 and 7 are used to monitor the Fault and Alarm contacts, with the inbuilt pull-up resistors switched on. Use the NC switch contacts.


Example output from curl:
```
$ curl -v http://xxx.xxx.xxx.xxx
* Rebuilt URL to: http://xxx.xxx.xxx.xxx/
*   Trying xxx.xxx.xxx.xxx...
* Connected to xxx.xxx.xxx.xxx (xxx.xxx.xxx.xxx) port 80 (#0)
> GET / HTTP/1.1
> Host: xxx.xxx.xxx.xxx
> User-Agent: curl/7.47.0
> Accept: */*
> 
< HTTP/1.1 200 OK
< Content-Type: text/html
< Connection: close
< 
# HELP alarm_battery_voltage Last sampled voltage of alarm battery
# TYPE alarm_battery_voltage guage
alarm_battery_voltage 0.00
# HELP alarm_alarm_relay_output Last sampled state of alarm relay output
# TYPE alarm_alarm_relay_output guage
alarm_alarm_relay_output 1
# HELP alarm_fault_relay_output Last sampled state of fault relay output
# TYPE alarm_fault_relay_output guage
alarm_fault_relay_output 1
* Closing connection 0

```


Set your Prometheus monitoring up by including config like so:

```
scrape_configs:
  - job_name: 'alarms'
    scrape_interval: 10s
    static_configs:
      - targets: ['xxx.xxx.xxx.xxx:80']
```

Restart Prometheus (or reload config) to see the new data series being built for you.

The alarm monitor exposes the following metrics:

- alarm_battery_voltage
- alarm_alarm_relay_output
- alarm_fault_relay_output

Set up alerting with Alertmanager by adding config, for example:

```
ALERT AlarmFault
  IF alarm_fault_relay_output == 1

ALERT Alarm
  IF alarm_alarm_relay_output == 1

ALERT AlarmBatteryDischarging
  IF alarm_battery_voltage <= 12.0

ALERT AlarmBatteryLow
  IF alarm_battery_voltage <= 11.0

ALERT AlarmBatteryCritical
  IF alarm_battery_voltage <= 10.0

```

If you plan to monitor more than one alarm panel, making more specific alert messages is probably wise, for example:
```
ALERT AlarmFault
  IF alarm_fault_relay_output == 1
  ANNOTATIONS {
    summary = "Alarm {{ $labels.instance }} is in FAULT state"
  }

```


# Other Possibilities

Why not take advantage of Prometheus & Grafana to provide pretty graphs for previoulsy dumb devices?

This could be easily repurposed for:

- UPS monitoring
- Environmental monitoring
- Fish tank monitoring
- etc...
