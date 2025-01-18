import serial
import speech_recognition as sr
import traceback
from colorama import init, Fore

command_list = """
Список доступных команд:
1. включи систему
2. выключи систему
3. режим поилка
4. режим полив
5. вода иди
6. вода стой
7. таймер
8. период
"""

init(autoreset=True)

ser = serial.Serial('COM7', 9600)

def make_commands(text_input: str):
    text = text_input.strip().lower()
    if "статус" in text:
        ser.write(bytes('статус\n', 'utf-8'))
        status = ser.readline().decode('utf-8').strip("\n")
        print(Fore.BLUE + "Текущий статус: " + status)
        #print(list(status))
        return 1
    elif "включи систему" in text:
        ser.write(bytes('вкл все\n', 'utf-8'))
        print(Fore.GREEN + "Система включена")
        return 1
    elif "выключи систему" in text:
        ser.write(bytes('выкл все\n', 'utf-8'))
        print(Fore.GREEN + "Система отключена")
        return 1
    elif "режим поилк" in text:
        ser.write(bytes('поилка\n', 'utf-8'))
        print(Fore.GREEN + "Включен режим поилки")
        return 1
    elif "режим полив" in text:
        ser.write(bytes('полив\n', 'utf-8'))
        print(Fore.GREEN + "Включен режим полива")
        return 1
    elif "вода иди" in text:
        ser.write(bytes('вода иди\n', 'utf-8'))
        print(Fore.GREEN + "Вода включена")
        return 1
    elif "вода стой" in text:
        ser.write(bytes('вода стой\n', 'utf-8'))
        print(Fore.GREEN + "Вода отключена")
        return 1
    elif "таймер" in text:
        time = -1
        for elem in text.split(" "):
            if elem.isdigit():
                time = int(elem)
                break
        if time < 0:
            print(Fore.RED + "Время таймера не распознано. Попробуйте еще раз")
            return -1
        ser.write(bytes('таймер\n', 'utf-8'))
        ser.write(bytes(str(time), 'utf-8'))
        print(Fore.GREEN + "Таймер выставлен в " + str(time) + " секунд")
        return 1
    elif "период" in text:
        time = -1
        for elem in text.split(" "):
            if elem.isdigit():
                time = int(elem)
                break
        if time < 0:
            print(Fore.RED + "Время периода не распознано. Попробуйте еще раз")
            return -1
        ser.write(bytes('период\n', 'utf-8'))
        ser.write(bytes(str(time), 'utf-8'))
        print(Fore.GREEN + "Период выставлен в " + str(time) + " секунд")
        return 1
    else:
        s = "Команда не распознана\n" + command_list
        #print("\033[31m{}".format(s))
        print(Fore.RED + s)
        return None



r = sr.Recognizer()
mic = sr.Microphone()
sr.LANGUAGE = 'ru-RU'
print('Запись пошла ')
while True:
    with mic as source:
    #with sr.AudioFile("1.wav") as source:
        r.adjust_for_ambient_noise(source)

        audio = r.listen(source)

    try:
        text = r.recognize_google(audio, language='ru-RU')
        print(f'Вы сказали: {text}')
        make_commands(text)
    except Exception:
        #traceback.print_exc()
        #print("Я тебя не понимать( Говори четче! ")
        ...
