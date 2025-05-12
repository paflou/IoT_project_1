#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "net/linkaddr.h"
#include "sys/log.h"
#include "utils.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (10 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

static linkaddr_t leaf_node[255];
struct data_form form = {-1, -1, -1.0, -1.0};
int count = 0;
char temp[32], hum[32];

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(linkaddr_t)) {
    memcpy(&leaf_node[count], data, sizeof(linkaddr_t));

    LOG_INFO(": Got your message leaf node, your ID is %d", count);
    LOG_INFO_("\n");

  
    nullnet_buf = (uint8_t *)&count;
    nullnet_len = sizeof(count); 
    memcpy(nullnet_buf, &count, nullnet_len);

    NETSTACK_NETWORK.output(&leaf_node[count]);
    count++;
  }

  else if(len == sizeof(struct data_form)) {
    memcpy(&form, data, sizeof(struct data_form));
  
    double2str(temp, form.temp);
    double2str(hum, form.hum);

    LOG_INFO("Message: ID: %d Count: %d Temperature: %sC Humidity: %s%%\n",  form.ID,
                                                                                form.count,
                                                                                temp,
                                                                                hum);
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

while(1) {
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));  // Wait for timer expiration

  LOG_INFO("I'm the parent node, my MAC is ");
  LOG_INFO_LLADDR(&linkaddr_node_addr);
  LOG_INFO_("\n");

  nullnet_buf = (uint8_t *)&linkaddr_node_addr;
  nullnet_len = sizeof(linkaddr_node_addr);

  memcpy(nullnet_buf, &linkaddr_node_addr, nullnet_len);
  
/* Initialize NullNet EVERY iteration since we use the same buffer for aknowledging new leaf nodes*/

  NETSTACK_NETWORK.output(NULL);

  // Reset the timer for the next cycle
  etimer_reset(&periodic_timer);
}

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
