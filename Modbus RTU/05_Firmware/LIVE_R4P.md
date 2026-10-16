# R4-P — transition unique puis coexistence physique FRAM/FIFO

Mission TR2-20261016-R4P-PHYSICAL-COEXISTENCE. Autorisation limitée à TR2-001,
UID004A003D/38425018/20333157, ST-LINK003500463235510B37333439. Aucune politique
produit de récupération automatique n'est introduite ; R0 et V1 restent inchangés.

Préconditions : Flash2Mio double lecture c5bc344d…f5b26, FRAM262144 double lecture
f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1, capteur arrêté
relu sans écriture, archive intégrale récupérable. Copie sans perte du STARTED2/run2
dans tests/fixtures/r4p_started_snapshot.zlib ; les sauvegardes brutes restent hors
Git sous reports/TR2-r4p/hardware. Les preuves/échanges R4-D sont préservés.

## Plan et anti-rejeu

Le même moteur, média transactionnel, propriétaire FIFO et outil R4-M sont réutilisés.
Le seul nouveau captureur refuse tout état autre que le snapshot attendu : unique
STARTED2/1/code13/gen6/run2/config2/gen2/INTENTphase1, vrai dernier RENEW1/65535 COMPLETED4/0,
epochcurrent2/previous1/gen3/flags0, seulement slots0/127. La vérification SHA intégrale
précède ce contrôle sémantique. Aucune preuve causale de l'effet ancien n'existe :
on archive son indécidabilité ; aucun COMPLETED/28/29 fictif n'est écrit pour lui.

1. Firmware opt-in R4_BENCH+R4M_MAINTENANCE+R4P_COEXISTENCE, sans R4D, image distincte
   NON-PRODUCTION. Chaque boot CLOSED/WAIT, aucun UART/SD/serveur normal.
2. Trigger4 exige le SHA initial et token de backups vérifiés. Une seule isolation
   logique du journal via candidate+commit transactionnel payload/header/root.
   Epoch, deux INTENT authentiques128octets, configuration et provenance conservés.
   Historique[0,122880) et tail[245760,262144) jamais écrits. Ancien snapshot complet
   conservé hors cible. Journal d'adresses, quanta32 et lecture retour du média.
3. Frontend RETIRING : seul vrai RENEW2/65535 est admis. Ouverture après durable3,
   previous2/flags0/COMPLETED4/0 et INTENT128 inchangé ; ancienneclé2/1 refusée24/0write.
   L'INTENT run2 authentique impose la prochaine réservation run3, sans floor inventé.
4. START3/1/run3 terminal4/0 avant poursuite ; RENEW3->4 sous acquisition,
   service60s, STOP4/2, APPLY4/3/config3, START4/4/run4. Correction R4-D réellement
   présente dans le moteur ; aucun seuil relâché. work1000us/service2500us/gap5ms,
   premier mot20ms expérimental distinct du gap continu. Provider B3 non raccordé.
5. Après dernier START COMPLETED seulement : faute de service logicielle8 injectée,
   STOP réel indépendant de FRAM, aucune écriture supplémentaire ; pas panne électrique.
6. Si succès, reset et trigger5 readonly sur SHA final attendu par oracle des vrais
   stores : epoch4/config3/dernierSTART4/run4, aucun autostart ni écriture.
7. Sauvegarder FRAM finale, comparer les zones protégées, restaurer uniquement la
   Flash initiale par procédure normale puis relire2Mio. Aucun rembobinage FRAM :
   les nouvelles identités consommées restent réservées même en échec.

**Quarantaine indispensable :** le pont n'est pas une récupération produit sûre
si un firmware générique l'expose à des commandes. Le contrôle négatif conserve
ce fait visible. Ce probe n'a aucune entrée normale ; après reset CLOSED et trigger4/SHA
initial refusés sur tout état intermédiaire, trigger6 de reprise R4-M désactivé.
Aucun générique LIVE ne doit être chargé sur un pont interrompu. Les images M016/V1
ignorent LIVE ; tout nouveau STARTED arrête cette mission sans nettoyage ni retry.
Un échec avant RENEW n'autorise pas non plus une nouvelle isolation.

## Validation préparatoire et limites

Test139 sur snapshot authentique, réutilisant les fixtures R4-M/vrais stores R1/R2 :
142budgets de coupure du commit (payloadstride4096 et chaque octet header/root),
4phases RENEW, frontend fermé au reboot, INTENT conservé, anti-rejeu, run3/4,
STOP/APPLY et défaut de service post-COMPLETED. La métadonnée finale dérivée est
un **modèle RAM**, jamais une preuve capteur :
SHA a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9.
Les138tests R4-D déjà réussis portent sur le même cœur fonctionnel : pas relancés
sans modification de celui-ci. Nouveau test ciblé, sanitizer et cross-build dédiés.
Aucun succès physique annoncé sur la seule base du modèle, du build ou du flash.

Le transport et le HAL conservent leurs limites connues en faute ; les maxima
nominaux ne sont pas des WCET. Aucun SD, HP, RTU électrique, PC/SQLite/Web ou R6.
La restauration FRAM depuis une ancienne archive n'est pas un rollback autorisé :
préserver toutes les bornes epoch/run consommées et faire cadrer toute reprise.


## Correction démontrée après l'isolation unique

Le premier trigger4 publie l'isolation (52426octets) puis s'arrête avant RENEW,
work1075us>1000us. Aucun nouveau STARTED : le journal courant est vide et epoch2/run2
ne sont pas avancés. Snapshot intégral du pont :
713a953384f2688f8448989ba706bf0dcef33ace3f177a1ee90fa5eb63713ba0.
Les écritures sont arrêtées et ce snapshot est préservé, sans nouvelle isolation.

Diagnostic lecture seule : breakpointfailed, pile compare_protected, at=245760,
work1075us. Cause démontrée :3840itérations sans service pour ignorer LIVE dans le
scan protégé, puis première lecturetail. Correction : saut O(1)122880->245760 ;
aucun octet historique/tail omis, compteur139264octets comparés attendu par passage.
Ce défaut du harness est distinct du START/OBSERVE R4-D ; aucun seuil relâché.

Plan de poursuite borné à ce pont, dans la même autorisation d'isolation unique :
trigger7 readonly, ownershipfalse/0write, montage/contrôle du pont et comparaison
complète protégée sous budget. Si échec, pas de nouvelle écriture. Si PASS et double
snapshot exact, reset vers CLOSED puis trigger8+token vérifié : capture du pont
sans actif/last/journal, epoch2/gen3/previous1, INTENT128 authentique byte-identique
à l'archive et floor2. RETIRING autorise seulement le vrai RENEW2->3 ; **aucune
seconde isolation**. Trigger6 reste désactivé, aucune reprise automatique au boot.
La correction est testée/compilée avant ce contrôle readonly ; la reprise d'écriture
n'a lieu qu'après démonstration que le dépassement est maîtrisé. Tout nouveau
STARTED indéterminé arrête la campagne, sans nouvelle exception ni nettoyage.


## Résultat physique du 16/10/2026 — code9f429e5

Trigger7 readonly PASS,0write/snapshot713a inchangé/work342us. Trigger8 démarre
le vrai RENEW2->3 sans réisolation puis six commandes terminales4/0 : START3/run3,
RENEW3->4 sousFIFO, RUN60,000989s, STOP/APPLYconfig3/START4/run4. Work913us,
service566us/gap822us,6454507XYZ/806814TS sur les deux runs, niveau25, aucune
erreur FIFO/stop signalée ;1310650octets/40975commandes physiques/75plages LIVE,
CSfermés40975fois. Deux scans complets278528octets protégés. Première isolation
séparée52426octets/1639commandes, jamais répétée. L'échec1075us reste archivé.

Faute logicielle de service8 post-COMPLETED : STOP235us, réaction242us,
aucune écriture supplémentaire, pas de nouveau STARTED indéterminé. STOP métier
portmax238us ; durée transactionnelle STOP42,082639s inclut les barrières.
Snapshot cible finala13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9
byte-identique à l'oracle (jamais écrit sur cible), racine37/epoch4/run4/config3.
Reset logiciel/recoveryreadonly PASS,0write/0autostart, snapshot intégral inchangé.
Flash initiale2Mio restaurée exactec5bc344d…f5b26, FRAM finale conservéea13b…a2ef9.

Ces observations satisfont le périmètre borné R4-P ; pas qualification indépendante
ChatGPT/R4globale/R6, ni power-cycle, WCET en faute électrique, providerB3 ou
communicationRTU validés. Premiermot6027487us mesuré depuis admission inclut
commitspré-effet, ce n'est pas un Ton capteur. Voir rapport/preuves de mission.
