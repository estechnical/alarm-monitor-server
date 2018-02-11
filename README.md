# alarm-monitor-server
Arduino based, Prometheus compatible metrics server for fire alarm panel

The project creates a Prometheus compatible metrics endpoint to allow timeseries data to be collected about the status of a fire alarm panel.

To make use of this, open the AlarmMonitorServer.ino file with arduino IDE.

Edit the IP address, choosing a free IP address suitable for your local network that is outside of DHCP allocated address space.

The hardware required depends on the alarm box itself. My alarm has relays for fault and alarm condition with NO/NC contacts available.
The resistor divider of 10k and 4.7k provides a roughly 15v range mapped onto the 0-5v range of analog input zero, so it suitable for monitoring a 12v SLA battery.

Digital pins 6 and 7 are used to monitor the Fault and Alarm contacts, with the inbuilt pull-up resistors switched on. Use the NC switch contacts.

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
  IF alarm_fault_relay_output{instance="xxx.xxx.xxx.xxx:80"} == 1

ALERT Alarm
  IF alarm_alarm_relay_output{instance="xxx.xxx.xxx.xxx:80"} == 1

ALERT AlarmBatteryDischarging
  IF alarm_battery_voltage{instance="xxx.xxx.xxx.xxx:80"} <= 12.0

ALERT AlarmBatteryLow
  IF alarm_battery_voltage{instance="xxx.xxx.xxx.xxx:80"} <= 11.0

ALERT AlarmBatteryCritical
  IF alarm_battery_voltage{instance="xxx.xxx.xxx.xxx:80"} <= 10.0

```

