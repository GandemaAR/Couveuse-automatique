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
>Un Ventilateur  
>Le Moteur Pas à Pas

## DS18B20

Le DS18B20, utilise le protocole 1-Wire. Nous avons implémenté ce protocole afin de l'utilisé correctement.
>Lien : "https://simple-circuit.com/pic16f877a-ds18b20-temperature-sensor/"

## Ventilateur

Le ventilateur à pour rôle d'évacuer la chaleur excedante de la couveuse. Maintenant deux méthodes s'offre à nous pour son controle, soit avec un DAC soit avec un PWM.
laquelle choisir????

>On utilise le PWM parce qu'il est beaucoup plus efficace, plus simple, moins cher et parfaitement adapté à la commande de puissance requise par un moteur. Le DAC est quant à lui idéal pour les signaux analogiques de précision (audio, instrumentation).

Explications détaillées :
1. Efficacité et Pertes d'Énergie (La raison principale)
PWM (Modulation de Largeur d'Impulsion) : Le signal est toujours soit à l'état HAUT (pleine tension, ex: 5V) soit à l'état BAS (0V). Lorsque le transistor/MOSFET de puissance qui conduit le moteur est saturé (ON), sa résistance est très faible, ce qui minimise les pertes par échauffement (Pertes = R * I²). Quand il est bloqué (OFF), le courant est nul, donc pas de pertes non plus. Le moteur "voit" une tension moyenne proportionnelle au rapport cyclique.

DAC (Convertisseur Numérique-Analogique) : Il produit une tension analogique variable (ex: 2.5V). Pour faire varier la vitesse, on utiliserait cette tension pour commander un étage d'amplification linéaire (comme un transistor en mode linéaire). Dans ce mode, le transistor dissipe énormément de puissance sous forme de chaleur car il se comporte comme une résistance variable (Pertes = (V_in - V_moteur) * I_moteur). C'est extrêmement inefficace, surtout pour les moteurs consommant du courant.

2. Simplicité de Mise en Œuvre et Coût
PWM : C'est une fonction matérielle intégrée dans presque tous les µC. Elle est générée "gratuitement" par un timer, sans charger le CPU. Le circuit de puissance est simple : un transistor/MOSFET et une diode de roue libre.

DAC : Très peu de µC d'entrée/moyenne gamme en intègrent un. Il faut donc ajouter un circuit externe (un circuit intégré DAC), ce qui augmente le coût, la complexité et la taille de la carte. De plus, il faut ensuite un étage amplificateur de courant pour piloter le moteur.

3. Courant et Puissance
Les moteurs demandent souvent des courants importants ( > 100mA). Un DAC typique d'un µC ne peut fournir que quelques mA. Le signal PWM, lui, pilote directement la gate d'un MOSFET de puissance qui peut lui, conduire des dizaines d'ampères avec très peu de pertes.

4. Réponse du Moteur
Un moteur à courant continu a une inductance. Cette inductance lisse naturellement le signal PWM pour fournir un mouvement relativement doux. L'inertie mécanique du rotor fait le reste. Un véritable signal analogique pur n'apporte donc aucun bénéfice notable pour le contrôle de vitesse basique.

5. Fréquence et Bruit
En choisissant une fréquence PWM suffisamment élevée (au-delà de 20 kHz typiquement), on place le bruit de commutation en dehors du spectre audible, ce qui élimine les sifflements gênants.

Quand utiliserait-on un DAC alors ?
Dans des applications de motorisation très spécifiques où la précision du courant ou la finesse du mouvement est critique, et où on utilise une boucle de contrôle sophistiquée. Exemple :
1. On peut utiliser un DAC pour définir la consigne de courant de référence pour un contrôleur de moteur plus intelligent (qui utilisera lui-même du PWM en interne).

2. Pour piloter l'excitation d'un moteur pas-à-pas en micro-pas avancés, où on a besoin de formes d'onde sinusoïdales très précises pour le couple et la réduction des vibrations.


Nous allons donc utiliser le PWM. Un mosfet sera utiliser ce qui nous permettra commander la vitesse du moteur que nous voulons(5V, 12V; etc).

Critères de Sélection pour votre Couveuse
Faible Tension de Commande (Gate Threshold Voltage Vgs(th)) : Votre µC (Arduino, STM32, ESP32, etc.) fournit du 3.3V ou 5V. Le MOSFET doit s'ouvrir complètement à cette tension.

Faible Résistance à l'état passant (Rds(on)) : Pour minimiser les pertes et l'échauffement, surtout si le moteur consomme du courant.

Courant Max (Id) : Doit être largement supérieur au courant du moteur.

Tension Max (Vds) : Doit être supérieure à votre alimentation (12V typique).

Montage en Bas (Low-Side) : On place le MOSFET entre le moteur et la masse (GND). C'est plus simple et sûr avec un µC.

Protection par Diode de Roue Libre : Intégrée au MOSFET (c'est un MOSFET "à corps diode").

Recommandations Concrètes (Modèles Courants)
Voici des MOSFETs "Logique-Level" (pilotables en 3.3V/5V), faciles à trouver et très utilisés.

Pour un Petit Ventilateur PC (12V, ~0.1 à 0.3A)
IRLZ44N : Un classique intemporel. S'ouvre bien à 5V, supporte jusqu'à 55V et 47A. Très polyvalent et pardonne beaucoup d'erreurs. Un peu surdimensionné, mais parfait pour apprendre.

STP55NF06 : Similaire, 60V, 55A. Excellente option.

FQP30N06L : 60V, 32A. Très bon pour les débutants, disponible chez tous les revendeurs.

Pour un Ventilateur plus Gros (12V, >0.5A) ou pour une Optimisation
IRLBS20 (ou IRLB8743) : Ce sont des champions. Résistance Rds(on) extrêmement faible même à 3.3V/5V. Ils chauffent beaucoup moins. C'est mon TOP RECOMMANDAITON pour un projet sérieux comme une couveuse.

AO3400A (SOT-23) : Si l'espace est limité et le courant modéré (~4A max). Très petit, parfait pour un ventilateur standard.

⚠️ Important : Vérifiez la fiche technique (datasheet) ! Regardez le graphique Rds(on) en fonction de Vgs. À Vgs=3.3V ou 5V, la résistance doit être très basse (typiquement < 0.02 Ω pour les modèles ci-dessus).

