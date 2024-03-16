

Message structure

  Priorization
    DESTINATION_ID:   A DEVICE_ID to which the message is intended specifically
    CHANNEL:          A string. Let all receiving devices organise the data by CHANNEL and SENDER_ID
    Some channel names are reserved:
      _DEF     DEFAULT           This means it does not have a channel with intention
      _COP     COMBAT OUTPOST    These messages are means for the field analysis, typically for sensor data aggregation
      _SYS     SYSTEM            These messages are meant for the system to maintain. Messages payload data have Key-Value structure


  MESSAGE_TYPE | DESC             | DATA
    00         | PING             | /
    01         | ACK              | SENDER_ID:MSG_ID
    02         | FRQHOP           | New frequency
    03         | GPS              | x.xxxxx:y.yyyyy
    04         | TXT              | You know what it is

  Message payload structure

    MESSAGE_TYPE , SENDER_ID , MSG_ID , CHANNEL , DESTINATION_ID , DATA , RETRY



  Examples

Ping:
 00,001,xx,channel,,0

// Only for destination
 ACK:
 01 231231231231 Channel 0 hfzhkfkezhfkhekjhezkjhekj 5

 // Only for destination
 Freq:
 02 231231231231 Channel 0 hfzhkfkezhfkhekjhezkjhekj 5

 // GPS
 Freq:
 03 1231231231231 Channel 0 hfzhkfkezhfkhekjhezkjhekj 5

 // Text
 Freq:
 04 1231231231231 Channel 0 This is text 5