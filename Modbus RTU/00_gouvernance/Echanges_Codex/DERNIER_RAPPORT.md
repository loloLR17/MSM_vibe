---
mission_id: TR2-20261016-R4D-START-TIMING-DIAGNOSTIC
status: IN_PROGRESS
base_ref: main
base_sha: 71ee541d6f534d993d439475c32bec5b03d7856c
initial_head: 88298cffa0403d80dfe95f534138401da0d9d3fb
result_sha: 7a6a8b924f4e616adfcb5fb2dc52c7ccc16297e0
created_at_utc: 2026-10-16T09:48:32Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-D — défaut de découpage reproduit, correction et diagnostic livrés

Travail technique terminé ; **IN_PROGRESS uniquement pour publication/relecture**.
START et OBSERVE forment un travail nominal continu de plus de1008us sans yield,
au-delà de la limite1000us. Leur séparation explicite donne un quantum cible
maximal de911us, sans relâcher les limites. Ce diagnostic ne résout ni ne nettoie
le STARTED epoch2/run2 ; aucune admission R2 ni écriture FRAM n'a été exécutée.
La Flash initiale est restaurée intégralement, FRAM byte-identique avant/après.

[Procédure, correction et limites](../../05_Firmware/LIVE_R4D.md) ·
[preuves brutes et manifeste](preuves/TR2-20261016-R4D/README.md) ·
[mesures](preuves/TR2-20261016-R4D/measurements.json) ·
[artefacts/configurations](preuves/TR2-20261016-R4D/build-artifacts.json).

## Préflight et préservation

Worktree neuf basé sur main88298cf, branche `codex/r4d-start-timing` ; aucun merge,
rebase, reset ou écrasement du dépôt historique.54anciens worktrees/HEAD préservés,
55au total ; deux stashes inchangés.2734fichiers protégés contrôlés par SHA, incluant
mission/gouvernance/gels/dettes/archives et preuvesR4-M. Mission et rapport R4-M
archivés byte-identiques à71ee541 sous archives/TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION.

ST-LINK003500463235510B37333439/NUCLEO-U575ZI-Q et UID004A003D/38425018/20333157
vérifiés. Flash2097152 etFRAM262144 relues deux fois, identiques aux backups R4-M.
Capteur relu arrêté : WHO7B, ODRoff, FIFO_CTRL4=80(modebypass), timestampoff.
GDB qualifié de lecture/RAM avant flash ; aucun HOTPLUG, aucun geste physique requis.
Backups complets récupérables : /home/lolo/dev/msm/reports/TR2-r4d/hardware/.

## Décomposition réellement mesurée

Probe avant correction **05033ffb1e6052c9f9744be80d667a2ee857bea7** ; transport et propriétaire réels,
SPI3 4MHz, cœur160MHz, driver officiel ST inchangé. Aucun UART/SD/commande R2.
L'ELF ne contient ni submit R2, ni commit du journal, ni maintenance, ni write FRAM.
Chaque image attend un triggerRAM et vérifie le SHA FRAM avant l'exercice.
Identifiants0x5234440000000001+n de diagnostic **volatils, non admis R2**.
Aucun halt GDB pendant l'exécution ;15s minimum avant lecture terminale.

| Exercice | START | OBSERVE | Intervalle combiné | STOP | Résultat |
|---|---:|---:|---:|---:|---|
| Direct0 |905,64us|102,34us|1008,26us|235,64us|effets/relectures conformes|
| Tracé1 |959,21us|106,93us|1066,43us|247,09us|48callbacks conservés|
| Direct2 |906,55us|102,34us|1009,18us|235,47us|contrôle de perturbation|
| Garde ancienne3 |904,98us|102,02us|1007,28us|234,38us|work1012us, erreur6, arrêt sûr|
| Découpage corrigé4 |907,09us|102,34us|1021,10us total|234,83us|**workmax911us**, service2us, aucune deadline|

Le total1021,10us du cas4 comprend les yields ; il n'est pas un quantum continu.
**Marge nominale observée89us**, pas WCET/borne industrielle. Le probe corrigé
exécute les mêmes frontières autour du vrai port avec callback de service idle ;
il ne fait pas tourner une commande R2 complète ni le service FRAM/FIFO sous charge.
Les138tests hôte contrôlent le vrai moteur modifié ; cette distinction est essentielle.

START comporte36callbacks SPI :9pour le STOP préalable,1WHO,23configuration,
3contrôles ; OBSERVE3, STOP9. Chaque callback vérifie les statuts/préclose et clôt CS.
Calibration128appels vides avant :63cycles direct,217wrapper sans append,293avec
append ; surcoût total230cycles=1,4375us/callback. Après ajout de l'injection,
surcoût1,56875us/callback. Les valeurs directes sont la référence temporelle ;
le surplus tracé≈58us pour39callbacks concorde. DWT/HCLK, sans calibration externe.

Sur l'authentique copie STARTED déjà présente : decode239,225us, encode227,2875us,
avec vérification byte-à-byte du réencodage. **Aucune phase/reçu nouveau construit**,
aucune admission ou persistance. START905–907 + OBSERVE102 + codec227 donnent
≈1235–1237us : déduction cohérente avec1241us de R4-M. L'ancienne pile exacte
n'étant pas capturée, il ne s'agit pas d'une reconstruction simultanée microseconde
par microseconde. En revanche, le dépassement par l'enchaînement START/OBSERVE seul
et son déclenchement sous garde sont directement reproduits.

## Correction minimale et causalité

Code **7a6a8b924f4e616adfcb5fb2dc52c7ccc16297e0**, sept lignes dans live_command_engine.c :
points de service avant effet, entre effet et observation indépendante et après
observation avant encodage/persistance. Contrôles SPI, CRC, phases, anti-rejeu et
seuils1000us/2500us/gap5ms inchangés ; contexteNULL = no-op historique.
Le STOP de sécurité reste indépendant de FRAM et ne dépend pas de ces checkpoints.

Une erreur entre effet et observation, ou avant reçu, conserve STARTED/INTENT et
arrête le capteur. Aucun effet incertain redispatché, aucun COMPLETED/ABSENCE_PROVEN
inventé. Un effet unique1100us reste refusé : le découpage ne borne pas un callback
HAL arbitrairement long. Les corps du driverST et du propriétaire FIFO sont inchangés.

## Tests et erreurs conservées

- Nouveau test138 : échec reproductible avant correction, PASS après. Modèle905us
  START+103us OBSERVE ; comparaison des262144octets finaux START/STOP aux mêmes
  opérations non coopératives ; deux fautes de service aux nouvelles barrières,
 0write supplémentaire, recovery9/no replay ; callback1100us toujours arrêté.
- **138/138 tests hôte PASS56,80s** après modification du cœur. Réexécution justifiée
  par le risque R2 ; includes1561oracles coupure/recovery,40positions d'erreur I/O
  du propriétaire et scénarios FIFO/STOP existants. Pas de répétition physique R4-M.
- **ASan/UBSan4suites ciblées PASS16,06s** : propriétaireFIFO, coopérationFRAM/FIFO,
  cooperative_work et live_effect_timing. Un premier build sanitizer globalO1
  a échoué sur `-Werror=clobbered` d'un ancien test longjmp hors tranche ; premières
  tentatives ctest NotRun, aucun PASS attribué. Les quatre cibles utiles ont ensuite
  été construites explicitement, sans suppression de warning/test/assertion.
  La suite sanitizer intégrale n'est pas déclarée validée.
- Cross-build canonique via `bash ./tr2_validate.sh --cross-build-only` et deux
  probes ARM PASS. Validation complète proportionnée composée de CMake/ctest138
  puis cross-build ; pas de second lancement inutile de138tests via le wrapper.
  BIN canonique V1 byte-identique6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9.
- Première configuration ARM refusée car argument registre commissioning manquant ;
  corrigée par le chemin réel REGISTRE_ATTRIBUTION_TR2.json, sans modifier le générateur.
  Logs de ces échecs conservés avec les sorties réussies et versions toolchain.

## Erreur capteur et limites HAL

Exercice5 nouveau, sans commande R2 : refus logiciel unique avant le11e callback
(lectureCTRL3_C), pas transfert matériel défaillant. START retourne6 ; nettoyage et
OBSERVE/STOP réels confirment arrêt, stop_error0, aucune saturation de trace.
Le sticky FIFOerror1 attendu reste visible ; il n'est pas effacé pour afficher PASS.
Ce n'est pas une panne électrique ni un timeoutHAL injecté.

Transport SPI3 : commande puis payload en polling, timeout20ms pour chaque appel,
aucun retry ; phasepayload seulement si commandeHAL_OK, CS relâché au retour,
statuts/compteurs/preclose contrôlés. HAL_GetTick commande le timeout : tick figé
ou IRQ masquées peuvent empêcher la borne temporelle. Une opération peut dépasser
1ms avant détection ; STOP partage le bus SPI3 et ne garantit pas la réussite
d'un arrêt électrique sur bus bloqué. **Pas de WCET fauteSPI/IRQ/CPU/pile qualifié**.

## État matériel final et empreintes

Flash initiale restaurée depuis backup frais,9pages/73728octets programmés par
procédure standard puis **2Mio comparés intégralement**. FRAM262144 relue après
chaque probe puis après restauration, toujours byte-identique. Historique/LIVE/tail
intacts, STARTEDepoch2/run2 toujours préservé, aucune identité persistante consommée.
Aucun mass erase/OptionBytes/RDP, aucun accèsSD/HP/COM7/RTU électrique.
Dernier état vérifié : M016 commissioning initial en attente, GDB détaché,
capteur arrêté par relectures du port réel avant restauration.

| Artefact | SHA256 |
|---|---|
| Flash initiale/finale2Mio |c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26|
| FRAM avant/après probes/restauration |f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1|
| Probe avant BIN |04b0d3c75bd4c031f7f46040ac4f670667ab0878790070d45f3ea65197b3a955|
| Probe après BIN |354b2e81f4577dbf4d9da001d140281b4471b0cf742875d61aa650621ad60e18|

## Suite distincte et publication

R4-D livre le diagnostic et sa correction ; **R4/R6 restent non qualifiés**.
La nouvelle campagne R2 en coexistence FRAM/FIFO, STOP/APPLY/reboot et provider
capteur demandent un contrat distinct, incluant la décision de traitement du
STARTED existant. Ne pas relancer les probes de maintenance pour nettoyer ce verrou.
Aucune extension de R0 ou annonce LIVE/protocole2/B3 n'est réalisée.

Journal ETAT_COURANT_TR2 et statut factuel R4 de la roadmap actualisés pour transmettre
ces acquis/limites à ChatGPT. Archives R4-M et2734fichiers protégés hash-identiques.
Dépôt historique propre à82169af7e76797731358852bec83bfb0ceb54ccf ; stashes
6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87 conservés.
Permissions exercées : développement/tests/commit/push normal versmain, flash/debug
qualifiés de probes puis restauration, sans écritureFRAM ni opération sensible.
SHA de code testés ci-dessus ; la relecture distante du bilan et la clôture restent
à consigner après push. Le SHA du dernier commit documentaire sera fourni dans la
réponse finale ; aucun statut ne vaut qualification indépendante ChatGPT.
