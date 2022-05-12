# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import serial
import threading
import time
import configparser
from concurrent.futures import ThreadPoolExecutor
import re
import pyowm
from datetime import datetime
from pathlib import Path
import dearpygui.dearpygui as dpg
import mysql.connector


print('[RUNTIME] Reading configuration file...')

config = configparser.ConfigParser()
config_path = 'settings.ini'

if Path(config_path).is_file():
    config.read(config_path)
else:
    config_set_defaults()


print('[RUNTIME] Connecting to Arduino...')

arduino = serial.Serial('COM3', 9600, timeout=5)


print('[RUNTIME] Connecting to OpenWeatherMap...')

owm = pyowm.OWM(config['weather']['api-key'])


pattern = re.compile("^([a-zA-Z]){3,9}$")


print('[RUNTIME] Connecting to MySQL...')

mydb = mysql.connector.connect(
    host = config['database']['host'],
    user = config['database']['user'],
    password = config['database']['password'],
    database = "arpy"
)

cursor = mydb.cursor()


def config_set_defaults():
    config.add_section('user')
    config.set('user', 'name', 'User')
    
    config.add_section('database')
    config.set('database', 'host', 'localhost')
    config.set('database', 'user', 'root')
    config.set('database', 'password', '')

    config.add_section('weather')
    config.set('weather', 'api-key', '')
    config.set('weather', 'base-url',
               '')
    config.set('weather', 'place',
               '')

    with open(config_path, 'w') as configfile:
        config.write(configfile)


def arduino_readSerial():
    data = arduino.readline()[:-2].decode('utf-8')
    if data:
        print('[DEBUG] ' + data)


def arduino_buildString(command, data, params):
    data_ = "[" \
        + "(" + command + ")" \
            + "{" + data + "}" \
                + "%" + params + "$" \
                    + "]"

    return data_.encode()


def arduino_buildString_noparams(command, data):
    data_ = "[" \
        + "(" + command + ")" \
            + "{" + data + "}" \
                + "]"

    return data_.encode()


def arduino_buildString_command(command):
    data_ = "[" \
        + "(" + command + ")" \
            + "]"

    return data_.encode()

print_text = "PY_PRNT_TEXT"
print_scroll = "PY_PRNT_SCRL"
print_clear = "PY_PRNT_CLSC"
alarm_play = "PY_ALRM_PLAY"
alarm_stop = "PY_ALRM_STOP"
alarm_hour = "PY_ALRM_HOUR"


def arduino_displayText():
    arduino.write(arduino_buildString(print_text, dpg.get_value(input_txt_field), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_y)))
    

def arduino_scrollText():
    arduino.write(arduino_buildString(print_scroll, dpg.get_value(input_txt_field), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_x_end) + "," + dpg.get_value(input_txt_y)))
    
    
def arduino_testDisplayText(sender, app_data, user_data):    
    arduino.write(arduino_buildString(print_text, "Hello, " + get_name() + "!", str(0) + "," + str(0)) \
                  + arduino_buildString(print_text, dpg.get_value(input_txt_field), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_y)))


def arduino_displayName(sender, app_data, user_data):
    arduino.write(arduino_buildString(print_text, get_name(), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_y)))
    
    
def arduino_displayTime(sender, app_data, user_data):    
    arduino.write(arduino_buildString(print_text, retrieveTime(), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_y)))
    
    
def arduino_displayHour(sender, app_data, user_data):
    arduino.write(arduino_buildString(print_text, retrieveTimeHour(), dpg.get_value(input_txt_x) + "," + dpg.get_value(input_txt_y)))
    
    
def arduino_displayWelcome():
    time.sleep(2)
    arduino.write(arduino_buildString_command(print_clear) \
                  + arduino_buildString(print_text, "Welcome, " + get_name() + "!", str(0) + "," + str(0)) \
                      + arduino_buildString(print_text, "It is now: " + retrieveTimeHour() + ":" + retrieveTimeMinutes(), str(0) + "," + str(1)))

    
def arduino_clearScreen():    
    arduino.write(arduino_buildString_command(print_clear))
    
    
    
    
    
    


def retrieveTime():
    now = datetime.now()  # current date and time
    # currentTime = now.strftime("%d/%m/%Y,%H:%M")
    
    currentTime = now.strftime("%Y-%m-%d %H:%M:%S")
    return currentTime


def retrieveTimeYear():
    now = datetime.now()  # current date and time
    currentTime = now.strftime("%Y")
    return currentTime


def retrieveTimeMonth():
    now = datetime.now()  # current date and time
    currentTime = now.strftime("%m")
    return currentTime


def retrieveTimeDay():
    now = datetime.now()  # current date and time
    currentTime = now.strftime("%d")
    return currentTime


def retrieveTimeHour():
    now = datetime.now()  # current date and time
    currentTime = now.strftime("%H")
    return currentTime


def retrieveTimeMinutes():
    now = datetime.now()  # current date and time
    currentTime = now.strftime("%M")
    return currentTime


def getGreeting():
    if int(retrieveTimeHour()) >= 19:
        return "Good evening"
    elif int(retrieveTimeHour()) >= 12:
        return "Good afternoon"
    elif int(retrieveTimeHour()) >= 6:
        return "Good morning"
    else:
        return "Good evening"


def save_name():
    config['user']['name'] = dpg.get_value(input_txt_name)
    with open(config_path, 'w') as configfile:
        config.write(configfile)
        dpg.set_value(title_txt, getGreeting() + ", " + config['user']['name'] + "!")
        
        
def get_name():
    return config['user']['name']


def get_weather(place):
    cty = owm.weather_at_place(place)
    weather = cty.get_weather()
    return weather.get_temperature('celsius')['temp']






alarm_state = False


def mysql_add_alarm():
    sql = "INSERT INTO alarms (date, name, description) VALUES (%s, %s, %s)"
    val_date = dpg.get_value(input_alarm_year) + '-' + dpg.get_value(input_alarm_month) + '-' + dpg.get_value(input_alarm_day) + " " + dpg.get_value(input_alarm_hour) + '-' + dpg.get_value(input_alarm_minute) + '-' + str(00)
    val_name = dpg.get_value(input_alarm_name)
    val_desc = dpg.get_value(input_alarm_desc)
    val = (val_date, val_name, val_desc)
    cursor.execute(sql, val)
    
    mydb.commit()
        
    
    
def mysql_get_alarms():
    cursor.execute("SELECT * FROM alarms")
    return cursor.fetchall()


def mysql_get_alarm(id_):
    sql = "SELECT * FROM alarms WHERE id = %s"
    val = (id_,)
    cursor.execute(sql, val)
    
    return cursor.fetchone()


def get_next_alarm():
    cursor.execute("SELECT * FROM alarms ORDER By date ASC LIMIT 1")
    return cursor.fetchone()


def get_next_alarm_date():
    if get_next_alarm() is None:
        return ""
    else:
        return get_next_alarm()[1].strftime("%Y-%m-%d %H:%M:%S")


def get_next_alarm_name():
    if get_next_alarm() is None:
        return "No alarms active"
    else:
        return str(get_next_alarm()[2])


def get_next_alarm_desc():
    if get_next_alarm() is None:
        return ""
    else:
        return str(get_next_alarm()[3])
    

def check_for_next_alarm():
    global alarm_state
    while True:
        time.sleep(1)
        
        alarm_date = get_next_alarm_date()
        alarm_name = get_next_alarm_name()
        alarm_desc = get_next_alarm_desc()
        
        dpg.set_value(txt_next_alarm_date, "At: " + alarm_date)
        dpg.set_value(txt_next_alarm_name, "Next alarm: " + alarm_name)
        dpg.set_value(txt_next_alarm_desc, "Description: " + alarm_desc)
        
        if not alarm_state and get_next_alarm() is not None:
            t1 = datetime.strptime(alarm_date, "%Y-%m-%d %H:%M:%S")
            t2 = datetime.strptime(str(retrieveTime()), "%Y-%m-%d %H:%M:%S")
            difference = (t1 - t2).total_seconds()
            
            if (difference <= 0):
                delete_alarm()
                alarm_state = True
                arduino.write(arduino_buildString_command(alarm_play) \
                              + arduino_buildString_command(print_clear) \
                                    + arduino_buildString(print_text, alarm_name, str(0) + "," + str(0)) \
                                        + arduino_buildString(print_text, alarm_desc, str(0) + "," + str(1)))


def delete_alarm():
    sql = "DELETE FROM alarms WHERE id = %s"
    val = (get_next_alarm()[0],)
    
    cursor.execute(sql, val)
    mydb.commit()


def stop_alarm():
    global alarm_state
    if alarm_state:
        arduino.write(arduino_buildString_command(alarm_stop))
        arduino_displayWelcome()
        alarm_state = False


def arduino_test_alarm_desc():
    arduino.write(arduino_buildString_command(print_clear) \
                  + arduino_buildString(print_text, "Alert, " + get_name() + "!", str(0) + "," + str(0)) \
                      + arduino_buildString(print_text, str(mysql_get_alarm(1)[3]), str(0) + "," + str(1)))
        

def test_hour_clock():
    now = datetime.now()  # current date and time
    # currentTime = now.strftime("%d/%m/%Y,%H:%M")
    
    currentTime = now.strftime("%#I")
    print(currentTime)
    arduino.write(arduino_buildString(alarm_hour, "", currentTime))


def debug_callback(sender, app_data, user_data):
    print(f"sender is: {sender}")
    print(f"app_data is: {app_data}")
    print(f"user_data is: {user_data}")


dpg.create_context()


with dpg.window(tag="Primary Window", label="Arpy Window"):

    title_txt = dpg.add_text(getGreeting() + ", " + get_name() + "!")

    input_txt_name = dpg.add_input_text(label = "Name",
                   default_value = get_name(),
                   callback=save_name)
    
    with dpg.collapsing_header(label="Communication"):
        input_txt_x = dpg.add_input_text(label = "x",
                               default_value = '0')
        
        input_txt_x_end = dpg.add_input_text(label = "x (end)",
                               default_value = '1')
        
        input_txt_y = dpg.add_input_text(label = "y",
                               default_value = '0')
        
        input_txt_field = dpg.add_input_text(label = "Text Field",
                       default_value = '')
        
        btn_display_text = dpg.add_button(label="Print this message!",
                       callback = arduino_displayText)
        
        btn_display_text = dpg.add_button(label="Scroll this message!",
                       callback = arduino_scrollText)
        
    with dpg.collapsing_header(label="Alarms"):
        
        txt_next_alarm_name = dpg.add_text("Next alarm: " + get_next_alarm_name())
        
        txt_next_alarm_desc = dpg.add_text("Description: " + get_next_alarm_desc())
        
        txt_next_alarm_date = dpg.add_text("At: " + get_next_alarm_date())
        
        input_alarm_year = dpg.add_input_text(label = "Year",
                               default_value = retrieveTimeYear())
        
        input_alarm_month = dpg.add_input_text(label = "Month",
                               default_value = retrieveTimeMonth())
        
        input_alarm_day = dpg.add_input_text(label = "Day",
                               default_value = retrieveTimeDay())
        
        input_alarm_hour = dpg.add_input_text(label = "Hour",
                               default_value = retrieveTimeHour())
        
        input_alarm_minute = dpg.add_input_text(label = "Minutes",
                               default_value = retrieveTimeMinutes())
        
        input_alarm_name = dpg.add_input_text(label = "Name",
                       default_value = '')
        
        input_alarm_desc = dpg.add_input_text(label = "Description",
                       default_value = '')
        
        btn_alarm_add = dpg.add_button(label="Add alarm",
                       callback = mysql_add_alarm)
        
        btn_alarm_stop = dpg.add_button(label="Stop alarm",
                       callback = stop_alarm)
        
        btn_alarm_get = dpg.add_button(label="Get alarm",
                       callback = arduino_test_alarm_desc)
        
        btn_alarm_hourly = dpg.add_button(label="Test hourly alarm",
                       callback = test_hour_clock)
        
    with dpg.collapsing_header(label="Debug"):
        
        btn_debug_print_serial = dpg.add_button(label="Print serial",
                       callback = arduino_readSerial)
    
    with dpg.menu_bar():
        with dpg.menu(label="Display"):
            btn_menu_display_text = dpg.add_menu_item(label="Display text",
                           callback = arduino_displayText)
            
            btn_display_name = dpg.add_menu_item(label="Display name",
                           callback = arduino_displayName)
            
            btn_display_time = dpg.add_menu_item(label="Display time",
                           callback = arduino_displayTime)
            
            btn_display_hour = dpg.add_menu_item(label="Display hour",
                           callback = arduino_displayHour)

        dpg.add_menu_item(label="Clear screen",
                          callback = arduino_clearScreen)

dpg.create_viewport(title='Arpy', width=600, height=600)

dpg.setup_dearpygui()

dpg.set_viewport_vsync(False)

dpg.show_viewport()

dpg.set_primary_window("Primary Window", True)


# Alarm thread
x = threading.Thread(target=check_for_next_alarm, daemon=True)
x.start()


executor = ThreadPoolExecutor(4)


thread1 = executor.submit(arduino_displayWelcome)
                


while dpg.is_dearpygui_running():
    dpg.render_dearpygui_frame()


dpg.destroy_context()
arduino.close()