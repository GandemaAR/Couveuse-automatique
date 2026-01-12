# Couveuse-automatique

# Description et Etude du projet

Nous allons à partir du PIC16F877A et de quelques capteurs réaliser une couveuse qui va surveiller et régulé la température et l'humidié dans la couveuse. 
Nous avons 4 paramètre crutial à surveiller: la températue l'humidité la ventilation et le retournemment. 

## La température

## L'himidité

## La ventilation

## Le retournement

## Matériel

SHT30/SHT31 capteur de température et d'humidité

# Code

## Outils

Nous allons utilisé:
>Un PIC16F877A  
>Le Compilateur CCS C  
>Le DS18B20  
>Un Ventilateur 5V  
>Le Moteur Pas à Pas

## DS18B20

Le DS18B20, utilise le protocole 1-Wire. Nous avons implémenté ce protocole afin de l'utilisé correctement.
>Lien : "https://simple-circuit.com/pic16f877a-ds18b20-temperature-sensor/"