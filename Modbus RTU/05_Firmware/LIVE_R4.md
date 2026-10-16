# R4 — coopération FRAM/FIFO, livraison et qualification

Cette implémentation conserve les formats R0/R1/R2 et V1. Production canonique
reste V1 ; aucune capacité LIVE/protocole2 ni B3 physique n'est activée.

## Chaîne réelle et contrat d'exécution

- FRAM SPI1 : SYSCLK/prescaler64 dans main.c,2,5MHz si SYSCLK160MHz. Pilote
  synchrone sans DMA, WREN/RDSR/WRITE/WRDI, CS fermé par commande, pas de retry.
  Ancien timeout HAL10ms, gros appels découpés256octets mais CS maintenu.
- IIS3DWB SPI3 : transport R3-A réutilisé, owner unique, bursts<=128×7, contrôles
  FIFO/tags/timestamps/configuration avant consommation. SPI4MHz canonique.
- UART/TIM6 : RX interruptions priorité5, ring64événements, TX256octets par
  HAL_UART_Transmit_IT avec watchdog50ms. Copie/armement sous IRQ masquées ; son
  WCET avec interruptions et transport physique reste à mesurer.
- CPU partagé : SPI distinct ne rend pas les CRC/SHA/recovery non bloquants.
  Chaque phase R2 garde313916octets lus/52426écrits, minimum utile1,172s à2,5MHz.

`Tr2CooperativeWork` est un contexte explicite partagé par les deux médias R1.
Il porte service, horloge64us, arrêt sûr indépendant, taille de quantum, limites,
maxima observés et erreur sticky. Aucune instance globale, allocation, framework,
format NVM ou qualification matérielle implicite. Absent : comportement historique.
Le binding ne crée aucune autorisation d'écriture ; ownership LIVE exigé à chaque
commande physique après yield, et au staging logique comme auparavant.

Les API restent synchrones avec **points de service coopératifs à l'intérieur**
de leur pile d'appels, pas des futures/threads ni un moteur delta. Les phases R2
restent RESERVED/STARTED/effet/COMPLETED. Payload complet puis header image, validation,
publication root puis revalidation ; aucun succès anticipé. CRC et scans longs
sont incrémentaux, SHA compression64octets inchangée ; les copies et décodages des128entrées journal
sont séparés par des points de service CPU. Codecs fixes96/112/140octets restent bornés.
Les scopes R2/R1/média/SHA s'imbriquent ; idle entre opérations ne crée pas un
faux timeout. En service : réentrée de transaction/staging/commande refusée avant
mutation. Sur erreur de service, recul de temps ou dépassement de limite : arrêt
sûr, erreur sticky et refus ultérieur ; pas d'auto-reset ni de succès fabriqué.

Le callback peut drainer le FIFO et une quantité bornée d'événements RX/RTU sur
**images déjà publiées en RAM**. Il ne peut ni lancer NVM/journal imbriqué, ni
lire une autorité R1 temporairement en recovery, ni réentrer dans le même serveur
RTU si celui-ci est déjà en train de dispatcher la commande extérieure. Pour
RTU2 futur, capture immuable/admission différée dans main puis progression R2 en
devoir de fond sont nécessaires ; ne pas simplement appeler submit depuis le
serveur et repoller récursivement ce serveur pendant sa persistance. La capture
d'une nouvelle commande ne vaut pas admission ; nonterminal reste refus13/26.
Les latences complètes de commande durable peuvent rester grandes même si FIFO
et RX sont servis ; elles sont distinctes du temps d'un quantum ou d'une réponse.

STOP de faute passe par safety_stop sans dépendre d'une écriture NVM. STOP métier
conserve ses barrières durables et réserves126/127 ; aucune réussite STOP sans
preuve d'effet et journal. Un budget externe reste obligatoire, absent en production.
Le modèle l'admet seulement si erreur absente et réserve calculée depuis limites
work+service et niveau FIFO<512. Ce budget déclaré/testé de modèle n'est pas une
qualification du T1000 ou du STM32.

## Transport STM32 optionnel

`stm32_fram_storage_bounded_physical` refuse>64octets, réutilise les mêmes commandes
avec copie locale du contexte et timeout HAL2ms ; originalV1 reste10ms. Au plus
6appels HAL en écriture,2en lecture, WRDI/CS fermé même sur échec. Cela borne le
**paramètre** de timeout, pas un WCET physique : tick IRQ, HAL, sections critiques,
latence totale et arrêt SPI3 doivent être mesurés. Une opération peut dépasser
work_limit avant détection ; ce garde-fou arrête/invalide, il ne répare pas une
perte déjà survenue. Pas de budget physique autorisé par ce seul changement.

## Preuves logicielles et oracles

- Suite complète136tests hôte + cross-build ARM. ASan/UBSan sur quatre suites
  incluant métrologieR3-B, puis reprise ciblée justifiée par renforcement UART.
- 1561oracles R2 de coupure réutilisés, mêmes assertions, avec coopération active,
  phases et chaque octet des headers/roots, payload stride4096 ; ce n'est pas une
  coupure à chacun des52298octets. Recovery arrête sans redispatch, protège historique/tail.
- START/RENEW/STOP/APPLY :262144octets physiques **identiques** à la séquence
  non coopérative. CRC/layout/identité/payload/générations/publication inchangés.
- Modèle indépendant : producteur240003/8words/s, profondeur512 ; FRAM2,5MHz+20/40us
  par commande, SPI3 4MHz+10us par transfert,4usCPU par lecture de clock. Ce dernier
  est un paramètre de modèle, pas un coût ARM mesuré. Admission1mswork+2,5msservice,
  réserve106words, seuilFIFO vérifié ; quanta16/32/64testés.
- Vrai owner, stores, moteur et PDU/RTU sur transport simulé. UART115200/11bits,
  FC03B0 et FC10max255octets RO refusé02, alternés ; TX modélisé asynchrone avec
  attente de durée fil avant nouveau maître. Drain borné8événements/callback,
  backlog vérifié<64. Pas une qualification du driver IRQ/DE physique.
- Résultat renforcé q32 :624,225411s simulées,13859117XYZ,13renouvellements,39033réponses,
  FIFOmax19, RXqueueMax8, overflow0. q16/64 FIFOmax16/24, gapmax370/566us ;
  workmax544us et servicemax362us dans ces hypothèses. Latence enqueue FC10≈25ms
  inclut la réception255octets ; pas temps de transmission complet ni borne cible.
- Urgence injectée pendant NVM :272us modèle vers arrêt observé, erreur sticky,
  aucun nouveau write après échec. Panne FRAM arrêt indépendant. Sans coopération,
  même producteur/stores débordent : contrôle négatif conservé.
- Hal mock réel pilote : six/deux positions d'erreur, pas de retry, CS fermé,
  plafond64 et timeoutV1 inchangé. Monotone en recul, service trop long/échoué,
  budget refusé et réentrée sont testés.

Les timestamps R3-B demandent une corrélation capteur calibrée ; aucune datation
SPI/HAL arbitraire n'est fournie ici. Sans ce provider, B3 LIVE reste indisponible.
Les tests métrologie numériques antérieurs restent actifs, sans devenir des
mesures vibratoires physiques.

## État de qualification et architecture

Conservés : formats, stores, invariants, V1, parser/transport/capteur, interfaces
synchrones et oracles. Mutualisés : coopération explicite des médias/CRC/SHA,
aucun nouveau driver FRAM parallèle. Optionnel : interface HAL bornée ; pas de
changement de fréquence, DMA, pins ou profils par défaut. Aucun code déclaré mort.
Alternative journal delta : décision structurante seulement si cette voie est
insuffisante, jamais migration silencieuse.

La qualification physique R4 nécessite identité/Flash/horloges, sauvegarde et
propriété exclusive de [122880,245760), mesures WCET/quanta/IRQ/FIFO/STOP/RTU et
corrélation temporelle. Les preuves sur cible et limites effectivement obtenues
sont dans DERNIER_RAPPORT et ses preuves, séparées de ce modèle. Ne pas déclarer
R4 physique ni passerelle R6 acquis sur ces seuls résultats hôte/ARM.


## Qualification R4 sur TR2-001 —16/10/2026

Voir DERNIER_RAPPORT.md et preuves pour les étapes séparées. Préflight positif,
Flash/FRAM doublées et vérifiées, allocation LIVE exclusive contrôlée. Probe opt-in
au WAIT RAM, artefact NON-PRODUCTION ; modes1lecture/2R1R2/3recovery readonly.
Aucun raccordement UART/SD ni capability protocole2/B3.

Échecs conservés : timeoutHAL1ms corrigé2ms ; premier-mot absent avant5ms, distinct
du gap continu (probe20ms, ST Ton10ms typique sans maximum garanti). Préfixe FIFO
readonly180826XYZ/FIFOmax18/gap601us/service461us/read132us, premier burst11,043ms,
arrêt235us. RUN entier nonPASS : contrôle final interrompu à l'arrêt par GDB ;
FRAM ensuite relue intégralement identique. R1 migrate+APPLY réussis sans FIFO,
puis harness réordonné pour binder le contrôle R2 avant mount, sans répéter R1.

R2 cible :1030us>1000us pendant validation des copies/décodages d'une entrée journal,
reproduit par recovery readonly avec pile GDB. Points de service entre copies et
décodages ajoutés, formats/invariants/limite inchangés. Codef3ae3a6 :136tests/ARM
PASS,74,52s ; ASan/UBSan4suites PASS,141,52s,1561oracles de coupure préservés.
Modèle final q32 :624,225411s/13859117XYZ/13renewals/39033RTU/FIFO19/RXqueue8/overflow0.

Recovery physique corrigé PASS en6,275s :923428octets lus/0écrit, workmax907us,
limite1000us inchangée, statut9 indéterminé conservé, aucun redispatch et FRAM
identique. Pas de qualification écritureR2+FIFO sur cible ; STARTED reste durable
sans reçu permettant de conclure. **Blocage causal réel** : décision de maintenance
ou reprovisionnement LIVE explicite requise avant nouvelle campagne R2.
R0 n'autorise ni reçu inventé ni effacement pour restaurer. Timeouts SPI3/IRQ/STOP
en faute, provider capteur calibré, RTU électrique et pile/CPU total restent à qualifier.
T1000 seulCOM3/STLINK, aucun adaptateur RS485 détecté. Pas de budget production accordé.

Flash initiale M016 retrouvée **entièrement restaurée2Mio**,14pages modifiées reprogrammées
sans mass erase ; FRAM finale conservée, historique et tail byte-identiques.
Voir empreintes et logs dans le rapport. Aucun firmware successeur laissé actif.
Cartographie421modules ;32868lignes C/C++/47665tests àf3ae3a6 (+527/+336 depuis66c5e16).
Tailles ARM work64/acquisition1104/FIFO1072/engine488/store9656/config584/media96.
Probe text106212/data476/bss593928 ; canonique text172580/data480/bss602232 et BIN
inchangé6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9.
Cadres `.su` statiques O0 seulement, pas high-water de pile ni WCET garanti.
