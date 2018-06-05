//#######################################################################################################
//############################# Plugin 150: SDM630 Eastron Energy Meter #################################
//#######################################################################################################
/*
  Plugin written by:          Sergio Faustino sjfaustino__AT__gmail.com
  Plugin for SDM630-only by:  Andreas Mueller juamueller__AT__gmail.com
  This plugin reads available values of an Eastron SDM630 Energy Meter
*/

#ifdef PLUGIN_BUILD_DEV

#define PLUGIN_150
#define PLUGIN_ID_150         150
#define PLUGIN_NAME_150       "Eastron SDM630 Energy Meter"
#define PLUGIN_VALUENAME1_150 "Voltage"

boolean Plugin_150_init = false;

#include <SDM_V2.h>         	                            // Requires SDM library from Reaper7 - https://github.com/reaper7/SDM_Energy_Meter/
SDM<9600, 12, 13> Plugin_150_SDM;

boolean Plugin_150(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_150;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;     // connected through 2 datapins
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_150);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_150));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
          byte meter_model = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
          byte meter_baudrate = Settings.TaskDevicePluginConfig[event->TaskIndex][2];
          byte query = Settings.TaskDevicePluginConfig[event->TaskIndex][3];

          String options_model[1] = { F("SDM630") };
          addFormSelector(string, F("Model Type"), F("plugin_150_meter_model"), 1, options_model, NULL, meter_model );

          //String options_baudrate[6] = { F("1200"), F("2400"), F("4800"), F("9600"), F("19200"), F("38400") };
		  String options_baudrate[1] = { F("9600") };
          addFormSelector(string, F("Baud Rate"), F("plugin_150_meter_baudrate"), 1, options_baudrate, NULL, meter_baudrate );

          String options_query[10] = { F("Voltage L1 (V)"),
                                       F("Voltage L2 (V)"),
                                       F("Voltage L3 (V)"),
                                       F("Current Total (A)"),
                                       F("Power Total (W)"),
                                       F("Frequency (Hz)"),
                                       F("Energy Total (kWh)"),
                                       F("Power Factor Total (Number)"),
                                       F("Phase Angle Total (Degree)"),
                                       F("Voltage Line to Line Average (V)") };

          addFormSelector(string, F("Query"), F("plugin_150_query"), 10, options_query, NULL, query );

          success = true;
          break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
          Settings.TaskDevicePluginConfig[event->TaskIndex][0] = getFormItemInt(F("plugin_150"));
          Settings.TaskDevicePluginConfig[event->TaskIndex][1] = getFormItemInt(F("plugin_150_meter_model"));
          Settings.TaskDevicePluginConfig[event->TaskIndex][2] = getFormItemInt(F("plugin_150_meter_baudrate"));
          Settings.TaskDevicePluginConfig[event->TaskIndex][3] = getFormItemInt(F("plugin_150_query"));

          Plugin_150_init = false; // Force device setup next time
          success = true;
          break;
      }

    case PLUGIN_INIT:
      {
        Plugin_150_init = true;

//        SDM<2400, Settings.TaskDevicePin1[event->TaskIndex],
//                  Settings.TaskDevicePin2[event->TaskIndex]> Plugin_150_SDM;
        Plugin_150_SDM.begin();
        success = true;
        break;
      }

    case PLUGIN_READ:
      {

        if (Plugin_150_init)
        {
          float _tempvar = 0;
          String log = F("EASTRON: ");
          switch(Settings.TaskDevicePluginConfig[event->TaskIndex][3])
          {
              case 0:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_VOLTAGE1);
                  log += F("Voltage L1 ");
                  break;
              }
              case 1:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_VOLTAGE2);
                  log += F("Voltage L2 ");
                  break;
              }
              case 2:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_VOLTAGE3);
                  log += F("Voltage L3 ");
                  break;
              }
              case 3:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_CURRENT_TOTAL);
                  log += F("Current Total ");
                  break;
              }
              case 4:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_POWER_TOTAL);
                  log += F("Power Total ");
                  break;
              }
              case 5:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_FREQUENCY);
                  log += F("Frequency ");
                  break;
              }
              case 6:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_IMPORT_ACTIVE_ENERGY);
                  log += F("Energy Total ");
                  break;
              }
              case 7:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_POWER_FACTOR_TOTAL);
                  log += F("Power Factor Total ");
                  break;
              }
              case 8:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_PHASE_ANGLE_TOTAL);
                  log += F("Phase Angle Total ");
                  break;
              }
              case 9:
              {
                  _tempvar = Plugin_150_SDM.readVal(SDM630_LINE_TO_LINE_VOLTS_AVERAGE);
                  log += F("Voltage Line to Line Average ");
                  break;
              }
          }

          UserVar[event->BaseVarIndex] = _tempvar;
          log += _tempvar;
          addLog(LOG_LEVEL_INFO, log);

          success = true;
          break;
        }
        break;
      }
  }
  return success;
}

#endif
