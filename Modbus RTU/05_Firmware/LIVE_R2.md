# Moteur LIVE R2 — transactions epoch et persistance

Références normatives : [profil R0](../01_Specification_source/LIVE_R0/PROFIL_LIVE.md),
[migration](../01_Specification_source/LIVE_R0/MIGRATION_FRAM.md),
[B5 V1.1](../01_Specification_source/V1_1/bloc5_v1_1_transaction_epoch.md).
Ce document décrit l'implémentation et ses limites ; il ne modifie pas les gels.

## État livré et frontière de composition

Moteur portable réel `LiveCommandEngine`, store `LiveCommandStore`, journal
format 4, epoch TR2Q, reçus TR2E et accès B5 à 29 registres. Les tests emploient
les vrais codecs et transactions d'image R1 ; seul le transport FRAM et le
propriétaire d'acquisition sont simulés. Code compilé pour Cortex-M33 et lié dans
un ELF de contrôle distinct. Aucune exécution sur STM32 pendant R2.

**La production reste protocole 1.** Le modèle historique garde B5 de 20 registres,
son dispatcher et ses oracles. Le nouveau moteur n'est pas appelé depuis
`stm32_production_runtime_prepare`, `SystemRuntime` ou le serveur PDU historique.
Son adaptateur B5 n'est donc pas une extension implicite du routage RTU existant.
Aucun profil de production protocole 2 ni bit LIVE n'est annoncé.

Cette limite est voulue tant que l'ensemble des invariants n'est pas raccordé :
le FIFO coopératif STM32 actuel est une capture bornée de qualification, armée
par le banc et liée à un tampon fini, pas le propriétaire continu R3 avec
configuration appliquée et reçu d'effet. Son chemin de disponibilité B5 refuse
les I/O pendant capture ; il ne peut pas être transformé par un simple appel en
un chemin STOP/renouvellement qualifié. Le `VibrationSource` V1 ne porte pas non
plus de preuve causale run/configuration et la composition historique exige ses
stores de campagne. R2 n'invente ni cette preuve ni une disponibilité temporelle.

| Point réel | État / raccordement requis |
|---|---|
| B0 / commissioning STM32 | Lecteur historique V2/protocole 1 conservé ; identité R1 protocole 2 préparée, capacités sans bit4, non servie. Choisir explicitement une composition successeur et prouver propriété FRAM avant provisioning. |
| B1 / état système | Cycle LIVE disponible dans le moteur ; assemblage/projection système successeur non raccordés. Il faudra retirer tout campaign_id et qualifier READY/fautes à partir des vraies autorités. |
| B4 | R1 `apply` utilise maintenant un validateur de contrôle R2 explicitement attaché avant montage. L'API R1 sans ce port conserve son refus strict des états R2. Projection B4 existante, non routée en production. |
| B5 | Backend et accès 29 registres implémentés ; routage FC03/FC10/RTU de production toujours V1. Raccorder ce chemin uniquement dans le profil 2 cohérent, jamais en allongeant silencieusement V1. |
| B6 / B7 | Aucune fausse disponibilité SD ni nouveau diagnostic publié. Projections système/inventaire/diagnostic successeur et persistance des événements restent à raccorder. |
| Acquisition / B3 | Port causal START/STOP/observation prêt ; aucun port STM32 R3 continu installé, aucun calcul ni snapshot B3 simulé. Sans port complet, START est refusé avant admission. |
| Commandes 0..11 | V1 de production préservée. Dans le nouveau backend, APPLY 1 est raccordé à R1 ; effets 2..11 non raccordés, refus explicite 5/3 après contrôles, sans journal V1 partagé. Leur composition sémantique complète reste nécessaire avant exposition protocole 2. |

Il s'agit d'une livraison du moteur logiciel avec ces limites, pas d'un firmware
LIVE opérationnel ni d'une validation de l'ensemble R2/R3/R4/R6.

## Usage, propriété et admission

Allouer durablement config R1, deux médias partageant une candidate de 52298,
store et moteur ; ne pas déplacer/copier ces objets après initialisation. Un seul
propriétaire, sans réentrance ni mutation extérieure de l'image/caches. Initialiser
R1, puis `live_command_store_init` **avant montage** : il attache un lecteur complet
du namespace R2, pas un booléen de désactivation des contrôles R1. L'initialisation
R1 ordinaire conserve les contrôles R1 inchangés. Pas de nouvelle migration ni
formatage au boot. Une image R1 initiale valide est lisible par R2 ; R1 refuse les
états R2 ultérieurs plutôt que les interpréter silencieusement.

Le port `LiveAcquisitionPort` est une dépendance empruntée, sans implémentation par
défaut. START reçoit run_id et snapshot actif ; `observe` doit confirmer état,
run_id, configuration/génération et application matérielle réelle. STOP exige
l'observation arrêtée du même propriétaire. Une valeur programmée SPI ou un enum
seul ne suffit pas. Les callbacks ne doivent pas créer de campagne ni accéder à
SD. L'horloge terminale est optionnelle ; indisponibilité -> timestamp 0, bit valide
0, sans retarder la terminalité. Zéro avec bit valide 1 reste un instant valide.

`io_admissible` est obligatoire pour une admission : son absence vaut refus.
Le test utilise une autorisation de modèle, pas une mesure physique. Le futur
ordonnanceur doit tenir sa promesse avant de retourner vrai. Des réserves de
compteurs couvrent l'opération et STOP/renouvellement ; elles ne prouvent pas les
latences ni une réserve électrique.

Ordre : capture immuable, epoch, txid/couple 12-65535, lookup/retry/collision,
code, paramètres/confirmation, contexte puis admission. Epoch 0/retirée/inconnue
->23/24/25 ; collision ->27 ; retry exact avant toute revalidation métier.
Une autre transaction non terminale ->13, ou26 pour un nouveau renouvellement.
Les 384 lignes du corpus R0 contrôlent la fonction de décision réellement appelée
par l'admission ; les intégrations sur stores contrôlent les états réalisables.
Les états campagne/contradictoires de la table ne sont pas fabriqués comme des
campagnes réelles dans la composition LIVE seule.

L'accès B5 accepte les lectures partielles et écritures 5000..5007 ou5020..5021.
Un accès traversant RO est rejeté avant toute mutation. FC10 doit transmettre un
lot entier à `write_b5` ; il stage le lot avant capture. Submit s'efface, clear
remet aussi l'epoch de mailbox à0 sans toucher active/last. La projection des29
registres est construite depuis un snapshot logique en RAM, sous l'exclusivité
du propriétaire ; pas de promesse d'atomicité entre requêtes différentes.

## Journal, effets et renouvellement

128 slots, 2 copies×112 octets/slot dans15922..44593 ; tail7168octets à0.
0..125 ordinaires,126 STOP si ordinaires pleins,127 renouvellement. Aucune entrée
de l'epoch courante n'est évincée. La réutilisation commence seulement dans un
namespace explicitement retiré par TR2Q. CRC, formats, réservés, phases, générations,
identités et transitions sont contrôlés ; aucun wrap de compteur.

Un record RESERVED contient l'identité immuable. START réserve aussi son run_id
durablement dans TR2E dès RESERVED, même s'il est ensuite abandonné sans effet.
Chaque `step` franchit une phase vérifiée :

1. RESERVED durable, aucun effet ;
2. STARTED + intention TR2E durable ;
3. effet demandé une seule fois, observation positive, reçu journal/TR2E durable ;
4. COMPLETED durable puis succès/last publiés.

APPLY capture la préparation, refuse en acquisition avant I/O, et appelle le vrai
commit R1. Sa configuration/génération cible permet de réconcilier un commit
configuration durable même avant le reçu journal. Les autres commandes héritées
ne sont pas redirigées vers des effets V1 pour simuler une intégration.

Les deux copies journal et l'enveloppe atomique protègent aussi last : lors de
réutilisation d'un slot retiré, une ancienne copie portant le dernier terminal
est conservée jusqu'au nouveau terminal. Les mises à jour intermédiaires peuvent
réécrire **la même copie logique**, dans une nouvelle image physique atomique.
Pas d'écriture physique en place. Cela préserve last entre renouvellements
consécutifs sans ajouter de domaine dans une zone réservée.

Renouvellement : RESERVED, STARTED, activation TR2Q N+1 liée au journal, puis
COMPLETED et retrait du drapeau pending en un commit. Les deux dernières copies
TR2Q prouvent une activation +1 ou une terminalisation de la même transition.
Au point pending : current=N+1, active=(N,65535). Retry ne produit jamais N+2.
Après terminalisation, N est retirée ; last peut encore porter N. La mailbox n'est
pas réécrite automatiquement. EpochFFFFFFFF refuse le nouveau renouvellement26,
sans arrêt d'un LIVE déjà confirmé.

## Recovery et sécurité

Le boot tente l'arrêt sûr même si la FRAM est illisible, puis lit les autorités ;
il ne redéclenche jamais START/STOP métier. Cet arrêt ne prouve pas l'absence d'un
effet antérieur.

- RESERVED sans STARTED : COMPLETED6/28 avant publication du résultat.
- STARTED avec reçu d'effet exact et relié à la transaction : finalisation durable.
- STARTED avec preuve durable ABSENCE_PROVEN causalement liée :6/29. Le moteur
  n'invente pas cette preuve ; le test fournit un fixture d'autorité explicite.
- STARTED sans preuve suffisante :9, identité active conservée et nouvelles
  admissions bloquées. Pas de nouvelle epoch de contournement.
- Renouvellement activé/pending : terminaliser N+1, jamais N+2. Sans preuve
  d'activation/exclusion exploitable : epoch status5/current0 et active9.
- Erreur FRAM ou observation incertaine : arrêt de sécurité indépendant, aucun
  faux succès, aucun retry d'effet. Une relecture défaillante après un commit
  durable reste une incertitude jusqu'à recovery.

Last vient des terminaux durables (copies incluses), jamais des refus avant
admission. Les dates sont historiques : reboot, horloge ultérieure, clear ou
lecture ne les redatent pas. La copie d'un succès START historique n'implique
jamais une acquisition active au reboot.

## Preuves et ressources

Test `live_command_engine` : vraies transactions, 384 contextes, 126 admissions,
slots réservés, retry/collision/epoch/temps, fautes, APPLY et **1561 budgets de
coupure** dans10 étapes. Payload par pas4096, chaque octet des headers/publications,
et deux transactions pour APPLY ; ce n'est pas chaque octet de payload. Les
campagnes et effets capteur sont des modèles hôte clairement séparés du matériel.

ARM GCC14.2.1 -O0 : moteur480, store9656, configuration584, média96 octets.
Design autonome avec deux médias et candidate partagée52298 : **63210 octets** hors
alignement et objets des callbacks, +10144 par rapport au design R1. Pas de heap
interne. Cadres statiques observés : submit440, step400, commit store160 octets ;
à lire dans les `.su` conservés. Ce ne sont pas des bornes de pile complète/HAL/IRQ.

START/STOP nominal : quatre transactions d'image, chacune écrit52426 octets,
soit **209704 octets écrits**. Les relectures, hash historique et CRC sont synchrones
à chaque étape ; les volumes exacts du transport simulé figurent dans le log R2.
APPLY ajoute son commit de configuration, soit cinq transactions ; renouvellement
et recovery concluant sont également coûteux. Aucune valeur de temps physique
n'est déduite de ces octets. Sans ordonnanceur/budget effectivement qualifié,
ne pas autoriser ces I/O en coexistence FIFO réelle. R4 reste indispensable.
