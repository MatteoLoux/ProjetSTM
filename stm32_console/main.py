import serial
import os
import sys

valid_command = {
    "LED1 ON", "LED1 OFF",
    "LED2 ON", "LED2 OFF",
    "LED3 ON", "LED3 OFF",
    "CHENILLARD1 ON", "CHENILLARD1 OFF",
    "CHENILLARD2 ON", "CHENILLARD2 OFF",
    "CHENILLARD3 ON", "CHENILLARD3 OFF",
    "CHENILLARD FREQUENCE1", "CHENILLARD FREQUENCE2", "CHENILLARD FREQUENCE3",
    "q", "Q", "quit", "h", "H", "help", "c", "C", "clear"
}


def print_interface():
    print("\nCommandes disponibles :")
    print(" LED1 ON / OFF")
    print(" LED2 ON / OFF")
    print(" LED3 ON / OFF")
    print(" CHENILLARD1 ON / OFF")
    print(" CHENILLARD2 ON / OFF")
    print(" CHENILLARD3 ON / OFF")
    print(" CHENILLARD FREQUENCE1 / FREQUENCE2 / FREQUENCE3")
    print(" Q / q / quit : Quitter l'application")
    print(" H / h / help : Afficher la liste des commandes")
    print(" C / c / clear : efface le terminal")

def check_command(command):
    return command in valid_command

def main():
    serial_port = serial.Serial(sys.argv[1], 115200)
    print_interface()
    while True:

        user_input = input("").strip()

        if not user_input:
            print("Aucune commande rentrée")
            continue

        if not check_command(user_input):
            print("Commande invalide. Tapez 'help' pour la liste.")
            continue

        if user_input.lower() in {"Q","q", "quit"}:
            break
        if user_input.lower() in {"H","h", "help"}:
            print_interface()
            continue
        if user_input.lower() in {"C","c", "clear"}:
            os.system("clear")
            continue

        serial_port.write((user_input + "\n").encode())
        print("Commande envoyée !")

    serial_port.close()

if __name__ == "__main__":
    main()