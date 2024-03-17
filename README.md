
# Communication System Message Structure

## Prioritization

- **DESTINATION_ID**: A DEVICE_ID for which the message is specifically intended.
- **CHANNEL**: Organizes data by CHANNEL and SENDER_ID for all receiving devices.
  - Reserved Channel Names:
    - `_DEF` (DEFAULT): General-purpose messages without a specific channel.
    - `_COP` (COMBAT OUTPOST): For field analysis, typically sensor data aggregation.
    - `_SYS` (SYSTEM): System maintenance messages, typically key-value data.

## Message Types and Payload

- **MESSAGE_TYPE**: Indicates the purpose of the message (`PNG`, `ACK`, `HOP`, `GPS`, `TXT`).
- **DATA**: Varies based on MESSAGE_TYPE.

## Message Payload Structure

Formatted as `MESSAGE_TYPE,SENDER_ID,MSG_ID,CHANNEL,DESTINATION_ID,DATA,RETRY`.

## Examples Corrected

- **Ping**: `00,001,xx,_DEF,,/`
- **ACK (Only for destination)**: `01,231231231231,_DEF,,SENDER_ID:MSG_ID,5`
- **Frequency Hop (Only for destination)**: `02,231231231231,_DEF,,New frequency,5`
- **GPS**: `03,1231231231231,_DEF,,x.xxxxx:y.yyyyy,5`
- **Text Message**: `04,1231231231231,_DEF,,This is text,5`
