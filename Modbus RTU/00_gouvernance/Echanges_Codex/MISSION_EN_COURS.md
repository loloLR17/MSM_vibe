---
mission_id: TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL
status: READY
base_ref: main
base_sha: 85e7a1ce38fb7d79d10dcb779275ec5c05561e7b
created_at_utc: 2026-11-18
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# MSM TR2 — Qualification matérielle ciblée du circuit RS485 Isolator 2 Click

## Décision et périmètre

L'utilisateur demande une **preuve de fonctionnement physique**, en émission ET réception, du **chemin Nucleo / shield V2.3 / RS485 Isolator 2 Click MIKROE-3863 / câble A-B / adaptateur USB-RS485 / T1000**. Il refuse qu'un simple audit de schéma ou un succès ERC/DRC constitue une qualification. Tester **exclusivement ce circuit**, et conserver les essais FRAM, SD et capteur pour d'autres missions.

Banc déclaré : une Nucleo + shield installés, ST-LINK raccordé au T1000, adaptateur USB-RS485 raccordé au T1000. L'utilisateur a **retiré TOUS les autres breakouts du shield pour les protéger** : FRAM, microSD et MKIGIBV2 sont absents pendant la mission ; **seul le Click reste enfiché**. Ne pas réinsérer les autres modules, ni demander de nouvelle SD, ni lancer leurs drivers/commandes ; laisser les supports correspondants dépeuplés. L'utilisateur indique avoir choisi le shield sans l'anomalie de masse précédemment observée sur l'autre banc, mais sa mention « J8-11 et J8-13 » n'est pas une mesure indépendante des rails : vérifier les vrais points/connexions et ne pas confondre les références J8 (MKI) et J2 (Nucleo).

Le défaut de brochage SPI3 J8 du shield V2.3 **reste avéré** : ne jamais le déclarer résolu. Le retrait physique de MKIGIBV2 élimine son exposition directe pour cet essai, sans valider globalement le shield. La revue E-01 reste NO-GO pour le produit global et pour refabrication ; le présent contrat autorise **seulement un essai ciblé**, après levée explicite et prouvée des conditions de sécurité de ce circuit.

L'ancienne mission TR2-20261117-SHIELD-DUAL-BRINGUP révision 2 reste BLOCKED pour sa partie globale, avec audit E-01 livré et revue indépendante. Son contrat historique est archivé dans `Echanges_Codex/archives/TR2-20261117-SHIELD-DUAL-BRINGUP/MISSION_REV2_ARCHIVEE_AVANT_RS485.md`. Ne pas relancer cette mission ni remplacer son rapport historique par une qualification fictive.

## Exécution autonome attendue

1. **Préflight à froid, sans alimenter le Click avant vérification :** inspecter références réelles Nucleo ST-LINK/UID, shield V2.3, Click (HW REV visible), présence/absence physique des breakouts, soudures/contacts J9 et état de J5, rails et GND logic vs GND2 isolé ; faire mesurer/observer par l'utilisateur uniquement les points qui exigent une action physique. Vérifier absence de court-circuit 3V3-GND et alimentation correcte Nucleo/Click, stabilité mécanique des cartes sans entretoises. Contrôler le domaine VDDIO2 lié à PG4/PG7/PG8, les positions VCCSEL 3V3, J2 DE-/RE, J7 A-Y et J8 B-Z half-duplex ; TERM/BIAS selon topologie réelle. Identifier/adresser les fils Data+/Data− de l'adaptateur d'après sa référence et les marquages (conventions A/B pouvant varier), vérifier absence de liaison intempestive GND2↔GND et éviter tout double maître. **Stop ciblé** si préflight non démontré ; pas de mise sous tension préventive.
2. **Chaîne DE critique :** le schéma prévoit PG4/CN12-69 → J5 local5 → J9-3/DE. J5 Samtec était auparavant absent. Confirmer sa présence effective et la continuité sur le banc avant de revendiquer le pilotage DE via le shield. Si absent, décrire factuellement la borne manquante et demander uniquement l'intervention physique minimale réellement nécessaire ; une liaison de banc provisoire contrôlée peut être étudiée avec l'utilisateur mais **ne vaut pas PASS du routage J5/shield** et ne doit pas être improvisée ni créée par Codex sans confirmation du geste physique. Ne pas court-circuiter les sorties, ni fabriquer un faux PASS en forçant DE sans documenter.
3. Inspecter firmwares et outils existants, privilégier les drivers officiels STM32 et références constructeur MIKROE pour le fonctionnement élémentaire isolé. Préparer un **banc firmware limité LPUART/DE** (autres SPI/CS désactivés et aucune initialisation/écriture FRAM, SD, IIS3DWB). Identifier fraîchement UID, ST-LINK, build, image et banque de flash ; standard flash et debug autorisés sous garanties AGENTS.md, conservation FRAM/SD/Option Bytes/identité, récupérabilité et readback. Ne pas utiliser des images historiques qui déclencheraient un capteur absent ou un accès aux médias.
4. Tester réellement, séparément, (a) **réception** adaptateur USB-RS485 → paire A/B → Click → UART STM32, puis (b) **émission** UART STM32 → Click avec bascule DE → adaptateur ; vérifier les octets, séquences, CRC et timing, repérer inversions A/B et erreurs de framing. Puis lancer, si matériellement possible, plusieurs requêtes/réponses Modbus RTU V1 réelles (dont adresse connue, fonctions sans effet mémoire), aucune commande destructive, traçage hôte/cible. Distinguer un test UART brut des transactions Modbus et une réponse réelle d'un écho local/loopback adaptateur. Mettre en évidence ligne DE au Click, état réception après TX, et isolation fonctionnelle dans les limites du banc.
5. Examiner stabilité par répétitions raisonnables et erreurs/timeout sans endurance nouvelle. Toute défaillance ordinaire peut être diagnostiquée et corrigée localement dans le périmètre, sans refonte, sans révision PCB non sollicitée ni affaiblissement des tests. Une condition extérieure ou structurante conduit à BLOCKED avec preuves et action physique précise.

## Critères d'acceptation

- **PASS physique RX** : trames hôte émises mesurées et données reçues par STM32 via le vrai Click et le vrai shield, comparées octet à octet.
- **PASS physique TX** : trames produites par STM32, pilote DE réellement actif/relâché, observées au véritable adaptateur USB et comparées octet à octet ; preuve de la continuité PG4→J5→J9 si l'on qualifie cette connexion du shield.
- **PASS Modbus** : requêtes et réponses RTU réelles avec CRC valide, timings cohérents, aucune erreur silencieuse ; verdict séparé si seuls UART TX/RX passent.
- **Sans J5 fonctionnel ou sans configuration half-duplex maîtrisée :** pas de PASS bidirectionnel intégral du shield. Documenter BLOCKED/limité avec constat objectif et proposition de geste minimal.
- Ne jamais annoncer « shield complet qualifié », ni effacer le NO-GO SPI3 ; ne pas invoquer un PASS documentaire E-01 comme résultat physique.

## Sécurité, permissions, livrables

`target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Garde-fous AGENTS.md : SN/UID corrects, chaîne ST-LINK identifiée, flash standard seulement, pas de mass erase, Option Bytes, changement security/boot, accès destructif aux médias ou FRAM. L'utilisateur réalise seuls les gestes physiques (câblage/straps/mesures/connexion). Un seul MCU et maître RS485 à la fois, gestion des handles Windows/WSL, COM réel à redécouvrir ; aucune hypothèse que COM4 est encore l'adaptateur.

Publier traces UART/Modbus et données hôte, captures d'état DE, configuration cavaliers, SN/UID, port COM, build/flash/empreintes, exact périmètre testé et limites ; état final matériel clair, rapport `DERNIER_RAPPORT.md` avec statut DONE uniquement si critères contractuels démontrés, sinon IN_PROGRESS/BLOCKED factuel. Commit/push et vérification distante selon gouvernance. ChatGPT fera ensuite le contrôle indépendant des preuves et commits avant qualification.
