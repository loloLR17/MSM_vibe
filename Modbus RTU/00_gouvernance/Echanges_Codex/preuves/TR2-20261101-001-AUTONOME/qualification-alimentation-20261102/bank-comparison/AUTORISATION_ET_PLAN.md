# Extension humaine — comparaison bornée TR2-002, 02/11/2026

Après la proposition publiée au checkpoint 663df57b, utilisateur :
« Tu as mon autorisation. TR2-001 et TR2-002 sont tous les deux connectés au T1000 en ST-LINK, tu en disposes comme tu veux. TR2-002 a encore sa carte SD dans le lecteur. »

Cette décision étend ponctuellement la cible du contrat à TR2-002
SN0032001F3235510C37333439 / UID001B0046/38425018/20333157 pour la comparaison proposée :
inventaire, deux lectures Flash complètes identiques avant programmation, préservation RAM,
flash/debug standard d'un diagnostic sans SD, au maximum deux essais de 60s,
puis restauration et readback intégral du firmware initial. Aucun accès SD/FRAM,
pas de formatage, Option Bytes, mass erase, COM4 ni changement de protection.
La carte32Go reste physiquement insérée selon l'utilisateur, sans commande SD.
Le contrat ChatGPT n'est pas réécrit par Codex.

Variante explicite R3_SENSOR_COMPARE_002, OFF par défaut, exigeant le profil
sensor-only et exactement60s. Réutilise le diagnostic001 FS16, SPI3 8MHz, FIR107Q15,
100µs de garde temporelle et budgets inchangés. Seuls UID et départ RAM explicite
0x44335202 au WAIT diffèrent ; le profil001 conserve USER obligatoire.
Cible SN explicitement sélectionnée et UID vérifié avant chaque opération.

Comparaison entre deux chaînes physiques : un succès002 ne suffirait pas à
incriminer exclusivement le MCU001. Le capteur/câblage réellement présents ne sont
pas déduits de l'énumération ST-LINK. En cas de WHO non conforme, pas de START
ni d'escalade aveugle ; restaurer après préservation du résultat.
