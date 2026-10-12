# M014-H — Composition supervision et provisioning explicite

La cible est le ST-LINK 003500463235510B37333439 / STM32U575 ID0x482,
UID96 004A003D 38425018 20333157. Les seules écritures de commissioning
concernent la FRAM sauvegardée ; aucun accès SD dans ces profils.

## Autorités et périmètre de qualification

Le registre ASSIGNED fournit DeviceId1/unit1/TR2-000001/MSM. Le générateur
vérifie l'unicité du DeviceId et du UID entier et produit le header C et les
21 registres B0 attendus. La lecture B0 provient exclusivement du domaine
persistant V2 ; le tableau attendu ne sert qu'au contrôle avant boot.
Hardware1 est une version de profil projet, pas une mesure de révision MCU.
Firmware0.14.8 est la version attribuée à cette composition ; protocole1 est
V1 gelé ; capacités0x000C annoncent supervision/diagnostic. Acquisition et
stockage SD ne sont pas qualifiés, leurs bits sont à zéro.

**Configuration structurellement valide, activation différée.** Le mapping
V1 exige storage_mode1 et une limite positive. La configuration préparée
porte une limite1Mio, mais aucune zone de stockage n'est attribuée. Le contexte
off-boot utilise une limite de validation structurelle UINT32_MAX uniquement
pour exercer les contraintes normatives de champ et les codecs existants.
Cela n'est ni une capacité mesurée ni une autorité de stockage. Le manifeste
l'explicite. Le boot normal reçoit au contraire environment_known=false,
usable_capacity=0 ; ConfigurationStore retourne UNAVAILABLE, aucun snapshot
actif n'est fabriqué. La disponibilité réelle vaut false, READY et stockage
B1 restent à zéro. Les actions campagnes du port explicitement indisponible
refusent NOT_AVAILABLE et recovery signale UNAVAILABLE, sans fallback FRAM.

Cette voie dégradée met à disposition les autorités réelles de supervision
sans qualifier la configuration opérationnelle/acquisition/bulk. La future
activation exige allocation exclusive et capacité réellement utilisable,
validation de ressource et nouvelle mission ; aucune écriture SD autorisée ici.

## Séquence FRAM bornée

Avant flash manufacturing : préserver Flash/RAM, identifier UID/ST-LINK,
contrôler SHA BIN/ELF et commandes standard NORMAL qualifiées. Après CLI,
reset GDB seul, jamais HOTPLUG. Pas de maître Modbus ni trafic en manufacturing.

Le preflight firmware lit deux fois les262144 octets FRAM, compare les lectures
et CRC32 du backup référencé dans le manifeste, vérifie UID96/registre,
recovery V1 VALID génération10, slot1 [140,280) totalement nul et inspection
structurelle de tous domaines après préparation de la configuration en RAM.
Le preflight est readonly. Extraire `tr2_provision_before` et comparer intégralement
au backup/hash historique côté hôte avant tout déclenchement.

Seul un trigger RAM explicite `tr2_provision_trigger=0x4D313448` autorise une
unique tentative ; répéter tout preflight immédiatement avant les écritures.
La génération10 n'est ni EMPTY ni formatée. Transaction1 installe config dans
slot1 et publie génération11 ; slot0 marqueur0xAC reste identique. Transaction2
préserve exactement le préfixe11, ajoute l'identité96 et publie V2 génération12.
Les APIs existantes écrivent payload/header/superblock, six écritures totales
sur le succès propre. Readback/recovery exigés. Divergence/erreur → arrêt sans
retry ni nouvelle tentative au boot ; reset laisse trigger0. Une coupure
après transaction1 nécessite un nouveau plan de reprise explicite génération11.

Comparer après succès le backup complet à la simulation hors ligne effectuée
sur sa copie réelle, vérifier V2/gen12, identité/CRC/préfixe. Ne jamais faire de
rollback production vers binaire pré-V2. Diagnostic E sans médias reste une
restauration de diagnostic possible ; pas restauration des données par flash.

## Profils et preuves

Options CMake exclusives et OFF par défaut : TR2_STM32_MANUFACTURING,
TR2_STM32_PRODUCTION ; manif/registre explicites obligatoires. Production exige
TR2_STM32_RS485_TX_QUALIFIED=ON. Branche avant toutes routines harness E1/E4.
Les symboles HAL_SD_ReadBlocks/HAL_SD_WriteBlocks/Sdmmc2_Bringup doivent être
absents des ELF dédiés. Manufacturing n'initialise aucun transport UART.

Le contrôle host `platform/host/commissioning_plan_check.c` se compile contre
tr2_core et le header généré ; il lit une copie du backup physique, vérifie
refus sans autorisation/UID incorrect, générations10→11→12, B0 attendu,
conservation des autres domaines et configuration UNAVAILABLE au boot sans
stockage ; écrit uniquement un fichier hôte attendu, jamais la carte.

Les essais HP utilisent l'outil G inchangé et ses539 tests conservés. Préflight
OFFLINE, paquet nouveau hashé, maîtrise exclusive COM7/1152008E1/unit1,
9requêtes FC03 maximum, retry0 ; aucun GDB pendant trafic. L'option OFF par
défaut TR2_STM32_PRODUCTION_SILENCE_TEST ménage une seule fenêtre uptime90..180s
pour le timeout planifié. Les événements reçus sont consommés pendant cette
fenêtre : aucune réponse retardée à l'ancienne requête. Cela qualifie une perte
de disponibilité du répondant sur le vrai bus, pas un retrait physique de câble.
Reconnexion = fermeture/réouverture réelle du driver série, B0 neuf puis B1
neufs ; les anciennes données restent indisponibles entre les deux.
