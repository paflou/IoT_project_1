#include "contiki.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/linkaddr.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "utils.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (20 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */


static linkaddr_t parent_addr;

static int id= -1;             //initialize leaf ID
struct data_form form = {-1, -1, -1.0, -1.0};
int count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet unicast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(linkaddr_t) && !linkaddr_cmp((linkaddr_t *)data, &parent_addr)) {
//    memcpy(&received_addr, data, sizeof(linkaddr_t));
    memcpy(&parent_addr, data, sizeof(linkaddr_t));
    LOG_INFO("Parent node address = ");
    LOG_INFO_LLADDR(&parent_addr);
    LOG_INFO_("\n");
    
    LOG_INFO("Got your message, I'm a leaf node, my MAC address is  ");
    LOG_INFO_LLADDR(&linkaddr_node_addr);
    LOG_INFO_("\n");
    
    nullnet_buf = (uint8_t *)&linkaddr_node_addr;
    nullnet_len = sizeof(linkaddr_node_addr);
    memcpy(nullnet_buf, &linkaddr_node_addr, nullnet_len);

    NETSTACK_NETWORK.output(&parent_addr);
    LOG_INFO_("\n");
  }

  else if(len == sizeof(id)) {
    memcpy(&id, data, sizeof(id));
    LOG_INFO(": Got your message, my ID is %d", id);
    LOG_INFO_("\n");
    form.ID = id;                 // set id
    count = 0;                    // reset count (new parent)
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */

  nullnet_set_input_callback(input_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(linkaddr_cmp(&parent_addr, &linkaddr_null)) {
    printf("waiting for parent \n");
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
  };

  LOG_INFO("Starting measurements transmission\n");

  if(!linkaddr_cmp(&parent_addr, &linkaddr_node_addr)) {
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

      LOG_INFO("Activating Sensor\n");
      SENSORS_ACTIVATE(sht11_sensor);

      form.count = count;
      form.temp = temperature_int2double(sht11_sensor.value(SHT11_SENSOR_TEMP));
      form.hum = humidity_int2double(sht11_sensor.value(SHT11_SENSOR_HUMIDITY));
      
      char temp[32];
      char hum[32];
      
      double2str(temp, form.temp);
      double2str(hum, form.hum);

      LOG_INFO("ID: %d Count: %d Temperature: %sC Humidity: %s %% \n", form.ID,
                                                                      form.count,
                                                                      temp,
                                                                      hum);

      LOG_INFO_("\n");

      /* Initialize NullNet */
      nullnet_buf = (uint8_t *)&form;
      nullnet_len = sizeof(form);
      NETSTACK_NETWORK.output(&parent_addr);

      SENSORS_DEACTIVATE(sht11_sensor);
      count++;
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
