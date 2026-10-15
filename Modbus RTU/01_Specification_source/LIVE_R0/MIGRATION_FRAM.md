# R0 — Persistance successeur et rollback sans écrasement de l'historique

Statut **SPECIFIED_NOT_IMPLEMENTED**. Complément normatif de `PROFIL_LIVE.md`, exigences **R0-FRAM**, **R0-POWER**, **R0-IDENT**. Aucune écriture ni observation de la FRAM pendant R0. Les nombres ci-dessous proviennent du code à `2b9c020`, pas d'une lecture matérielle.

## 1. Layout effectivement déclaré

`05_Firmware/platform/stm32/stm32_production_board.c::geometry()` et `include/tr2/persistence/transactional_image_media.h` définissent FRAM 262 144 octets, superblocks de 64 octets aux offsets 0/4096, banques aux offsets 8192/65536 de 57 344 octets ; image logique V2=51 914 octets, header de 64 octets. Réserver par prudence **tout [0,122880)** à l'historique, y compris ses gaps : aucune écriture LIVE dans cette enveloppe. La borne 122880 est 65536+57344.

Layout logique historique, dérivé des headers `system_persistent_layout.h`, `configuration_store.h`, `campaign_repository_store.h`, `campaign_data_store_persistent.h`, `command_journal_bounded_slot.h` et `persistent_identity.h` :

| Domaine | Début inclus | Fin exclue | Taille |
|---|---:|---:|---:|
| Configuration 2 slots de 140 |0|280|280|
| Temps |280|298|18|
| Dépôt campagnes |298|4370|4072|
| Réservation données campagnes |4370|15922|11552|
| Journal V1 borné 256×2×70 |15922|51762|35840|
| Diagnostic dernier défaut+autotest |51762|51802|40|
| BootIntent |51802|51818|16|
| Identité V2 |51818|51914|96|

Il s'agit d'offsets **logiques dans une banque transactionnelle**, pas d'adresses physiques directes. Écrire un record logique via le média actuel peut alterner et réécrire une banque entière : conserver seulement le slot de configuration 140..280 ne garantit donc pas la préservation de l'historique physique. De plus `select_recovery_status` actuel donne priorité à UNAVAILABLE, même avec une autre candidate valide ; cette règle doit être spécialisée dans le successeur, jamais copiée sans examen.

L'identité historique `persistent_identity.c` vérifie version protocole exacte et masque 0xFFF0 : elle refuserait bit LIVE 4. Changer seulement le record configuration ne permettrait donc pas de démarrer protocole 2. Les garde-fous commissioning actuels imposent aussi un préfixe V1 inchangé et une image 51914 ; **ne pas contourner ces APIs pour leur faire publier LIVE**.

## 2. Allocation successeur distincte

Le successeur définit une seconde partition physique autonome :

| Zone LIVE | Début inclus | Fin exclue / taille réservée |
|---|---:|---:|
| SuperblockA, enveloppe |122880|126976 (record64, reste réservé)|
| SuperblockB, enveloppe |126976|131072 (record64, reste réservé)|
| BanqueA |131072|188416 (57344)|
| BanqueB |188416|245760 (57344)|
| Hors allocation LIVE, ne pas écrire |245760|262144|

Les intervalles ne se chevauchent pas et restent dans 262144. Ce calcul prouve **la possibilité géométrique**, pas la disponibilité réelle. Aucun utilisateur du tail n'a été trouvé dans la composition de production inspectée, mais cela ne prouve ni qu'il est vide ni qu'il appartient à LIVE sur la carte actuelle.

**Précondition R1 avant la première écriture :** identifier cible/FRAM, propriétaire exclusif, carte d'allocation versionnée et contenu réel de toute la zone à utiliser ; préserver et vérifier les données existantes proportionnellement au risque. Si contenu non attribué, propriétaire inconnu ou géométrie différente : aucune écriture, pas de formatage ni supposition « libre car non référencé ». Une mission matérielle doit autoriser explicitement ce provisioning prévu. L'allocation documentaire R0 n'autorise aucune écriture R0 et ne rend pas réutilisables des données utilisateur. Ce garde-fou est une condition d'exécution R1, pas une variante normative laissée à choisir.

Une implémentation LIVE n'écrit que cette nouvelle partition. L'ancien firmware qualifié garde sa géométrie originale et ne lit jamais la nouvelle partition. Les deux banques et superblocks historiques restent byte-identiques pendant toute exécution LIVE ; cela préserve les records anciens même après un nombre arbitraire de commits LIVE. Aucune modification OptionBytes/boot persistant n'est nécessaire : le choix de partition est une propriété de l'image firmware identifiée.

## 3. Format et autorités du nouvel espace

Nouvelle enveloppe transactionnelle format 3, distincte des formats média 1/2 historiques ; elle contient une image logique **52 298 octets**, header de 64 octets, CRC d'image et superblock CRC/génération selon les barrières transactionnelles existantes à porter explicitement. 52298+64<=57344 ; slack de 4 982 octets par banque, sans y inventer un domaine. Sélectionner d'abord le superblock de génération maximale dont le CRC et le header sont valides. Sa banque doit ensuite correspondre en version, taille, génération, identité et CRC ; sinon refuser le recovery sans fallback silencieux. Les superblocks plus anciens peuvent référencer une banque inactive déjà réécrite : ce décalage ancien ne doit pas invalider la racine courante. Deux racines de même génération avec contenu divergent constituent un conflit bloquant. Une candidate inactive non publiée est ignorée ; un superblock déchiré/invalide ne remplace pas la précédente racine valide. Ne pas réinterpréter une racine publiée inconnue comme EMPTY.

Le format physique du header/superblock reste1 (offset4U16), la version **logique** passe à3 (offset6U16). Headerimage64 BE : magic TR2I à0, versions4/6, generationU64 à8, taille52298 à16, header_size64 à20, CRCpayload à24, réservés28..59=0, CRCheader0..59 à60. Superblock64 BE : magic TR2M à0, versions4/6, generationU64 à8, image_index0ou1 à16, réservés17..19=0, taille52298 à20, header_size64 à24, réservés28..59=0, CRCsuperblock0..59 à60. L'expression « enveloppe 3 » désigne cette versionlogique3, pas une modification cachée du formatphysique1. La générationFFFFFFFFFFFFFFFF reste interdite, comme dans le média actuel ; atteindre la dernière génération utilisable interdit un nouveau commit sans wrap.

Les offsets 0..51913 sont conservés dans la nouvelle image, avec des lecteurs versionnés :

- Configuration format 2, taille 140/magic TR2C, offsets header/payload/CRC identiques au format 1 (CRC sur 0..135, stocké136..139). Format 2 implique schéma B4=2 et ses tuples ; la version ne réside pas dans le payload CRC B4. Lecture format 1 permise pour comparaison/migration, pas de conversion silencieuse vers LIVE. IDs/générations/révision conservés ; nouvelle configuration LIVE est une nouvelle application, pas une recovery du record campagne.
- Identité format 2, taille 96/magic TR2D ; mêmes positions pour UID96/DeviceId/unit/serial/hardware, protocole 2 à44..45, capacités à46..47 (bits0..4autorisés), CRC 0..91stocké92..95. Les attributs physiques/identifiants restent identiques ; seules version/capacités/firmware et génération d'identité sont explicitement attribuées par le plan de migration. Ancien format 1 reste dans l'ancienne partition. L'ancien décodeur refuse format 2, aucune tentative de le rendre permissif.
- Temps, campagnes historiques et diagnostics : copie contrôlée de l'image récupérée, sans changer les IDs/labels/préfixes durables. Ils restent des autorités distinctes, pas des mesures LIVE. BootIntent hérité ne réenclenche aucune acquisition. Journal V1 n'est pas converti en journal epoch en inventant une causalité ; son original reste dans l'ancienne partition.

La région journal existante de 35840 octets contient le **nouveau journal format 4**,128 slots×2 copies×112=28672 octets à15922..44593, puis7 168 octets réservés à0 dans le nouvel espace. Le format 4 ne remplace pas un header du journal V1 à la même adresse physique : toute la nouvelle image est dans la partition LIVE. Aucun namespace V1 n'est amalgamé avec epoch 1 LIVE.

Record journal 112 octets BE : magic TR2J[0..3],format 4[4..5],size112[6..7],generationU64[8..15],epochU32[16..19],txidU16[20..21],codeU16[22..23],param1[24..25],param2[26..27],param3U32[28..31],confirm[32..33],phase[34..35],finalstatus[36..37],result[38..39],detail[40..41],timestampvalid[42..43],timestampU32[44..47],runidU64[48..55],configidU32[56..59],configgenerationU32[60..63],effectstate[64..65],réservés 0[66..107],CRC32[108..111]sur 0..107. phase 1RESERVED/2STARTED/3COMPLETED ; effectstate0 aucun,1 intention,2 effet observé,3 absence prouvée. Un enum seul ne remplace pas la preuve et les barrières ; generation/CRC/identité/causalité doivent toutes concorder.

Slots 0..125 pour transactions ordinaires ; slot 126 réservé à un STOP 4/14 quand il n'existe plus de slot ordinaire ; slot 127 réservé au renouvellement 12. Aucun txid admis de l'epoch courante n'est évincé. À 126 transactions ordinaires, refuser les nouvelles admissions non STOP par 5/7 ; STOP dispose encore d'un slot, renouvellement aussi. Après STOP réservé et renouvellement durable, nouvelle epoch/nouveau namespace. Il est permis de renouveler avant txid 65534 ; uniquement si toutes les transactions admises sont terminales et budget IO sûr. Ne pas bloquer la supervision courante pour un namespace saturé ; client peut explicitement renouveler pendant LIVE si ordonnancement qualifié. Une rupture FRAM déclenche l'arrêt sûr, jamais un ACK de commande inventé.

Extensions logiques :

| Autorité | Offsets / taille | Record 64 BE |
|---|---|---|
| EpochA/B |51914..52041,2×64|magic TR2Q,format 1,size64,generationU64,currentepochU32,étatU16,réservéU16,previousepochU32,renewtxidU16,flagsU16,transitionidU64,réservés20,CRC sur 60octets|
| Effet LIVE A/B |52042..52169,2×64|magic TR2E,format 1,size64,generationU64,epochU32,txidU16,codeU16,runidU64,configidU32,configgenerationU32,phaseU16,réservés18,CRC sur 60octets|
| ProvenanceA/B |52170..52297,2×64|magic TR2M,format 1,size64,generationhistoriqueU64,SHA256imagehistorique32,UID96,CRC sur 60octets|

Dans TR2Q, état reprend epoch status 0..5 V1.1 ; flags bit0 activation N+1 en attente de terminalisation, autres 0. Le domaine historique LIVE démarre explicitement à1 dans le provisioning, puis chaque nouvelle epoch provient d'un renouvellement durable+1 avec preuve de transition. La frontière de retraite des epochs est cette chaîne autoritative, pas une simple comparaison numérique d'une requête inconnue. Époque 0 ou corrompue jamais reconstruite depuis le journal. Au plafond generation/epoch, aucun wrap.

Dans TR2E, phase 1=INTENT_DURABLE,2=START_OBSERVED,3=STOP_OBSERVED,4=ABSENCE_PROVEN. INTENT n'est pas une preuve de capture ; runid est réservé durablement avant effet et jamais réutilisé après abort. Le reçu observé associe identité commande et configuration à l'acquittement réel de l'état capteur. L'autorité de transaction reste le journal, celle de l'epoch reste TR2Q. TR2M lie l'espace LIVE à une version réelle de l'historique, pas à un snapshot constant compilé supposé actuel.

## 4. Migration explicite et coupures

1. Préflight readonly des deux espaces, UID/DeviceId/unit, versions/CRC/générations ; refuser corps illisible ou attribution tail indéterminée. Capturer les empreintes de l'enveloppe historique et de l'image autoritative. Pas de backup répétitif sans changement risqué.
2. Construire une candidate dans la nouvelle partition : copie contrôlée d'historique, identité format 2/protocole 2, provenance TR2M, epoch 1 explicite, journal nouveau vide, effet LIVE absent. Ne pas activer une configuration LIVE au nom d'un ancien record campagne ; conserver les records format 1 en lecture historique tant qu'une nouvelle configuration n'a pas été appliquée. Les callbacks boot V1 qui bloquent toute active à cause de la SD sont remplacés par la politique ciblée du successeur, pas contournés par une capacité inventée.
3. Écrire payload et header dans une banque LIVE inactive ; vérifier lecture intégrale et CRC ; publier le superblock LIVE en dernier. Les API publier_v2 historiques ne conviennent pas à l'enveloppe 3 : une implémentation séparée/port explicite revu est requise. Aucune écriture dans[0,122880) ou[245760,262144).
4. Après barrière transactionnelle et recovery conforme, publier B0 protocole 2 ; état NO_CONFIG/READY, jamais LIVE. Nouvelle préparation LIVE+CRC puis APPLY par commande pour obtenir une active. Départ uniquement sur START_LIVE distinct.
5. À chaque commit de configuration, sélectionner la candidate du schéma supporté la plus récente **avant** la validation dynamique du mode ; un ancien record campagne UNAVAILABLE ne masque pas une nouvelle LIVE valide. Aucun fallback vers campagne en réponse à la corruption d'une LIVE. Si la configuration référencée par une transaction d'activation durable est irrécupérable, retirer l'active et signaler, pas choisir arbitrairement un ancien record.

| Coupure | Oracle de reprise |
|---|---|
| Avant première écriture LIVE |historique inchangé ; pas de LIVE publié|
| Pendant payload/header, avant superblock valide |aucune autorité candidate ; ancien LIVE valide s'il existe, sinon LIVE indisponible ; historique récupérable par ancien firmware|
| Pendant écriture superblock |CRC/génération choisissent un commit intégral ou aucun ; jamais active partielle ni mélange de banques|
| Après superblock, avant projection |recovery restaure identité/configuration engagées, acquisition arrêtée|
| Pendant APPLY, avant commit actif |ancienne active intacte ou indisponibilité explicite si NVM non fiable|
| STARTED avant reçu réel |pas de redispatch ; recovery 9 si effet antérieur indécidable|
| Après reçu, avant COMPLETED |finalisation seulement si reçu autoritatif démontre effet ; pas de réexécution|
| Après COMPLETED |retry exact rend résultat+timestamp historiques ; acquisition reste arrêtée après reboot|
| Renouvellement après N+1 avant COMPLETED |current N+1, active(N,65535), jamais N+2 au retry|

Le modèle documentaire de coupures ne prouve ni atomicité électrique FRAM ni timing SPI. R1/R2 doivent injecter les coupures dans les vrais stores et les barrières, puis R6 les qualifier matériellement dans son périmètre.

## 5. Rollback et compatibilité

Rollback du **firmware vers le cumulatif V2 historiquement compatible** : après arrêt sûr et processus de flash qualifié d'une mission future, l'ancien firmware retrouve son ancienne partition byte-identique, son identité protocole 1 et ses records d'origine. Cela ne prouve pas la présence d'un stockage campagne et ne rend pas M014-H acquisition active. Pas de rollback vers un firmware pré-V2 interdit par le commissioning M014-H. La partition LIVE demeure intacte mais inutilisée ; aucun effacement pour « restaurer ».

Si l'ancien firmware a ensuite modifié sa partition, TR2M ne correspond plus au generation/SHA de l'historique. Retour au firmware LIVE : **refuser l'activation de l'ancien espace LIVE**, ne pas recopier automatiquement des changements ni supposer des IDs compatibles. Nouveau plan explicite de réconciliation/migration nécessaire, avec sauvegardes proportionnées et attribution des deux autorités. Un rollback n'est donc pas une synchronisation bidirectionnelle ni un rollback des observations SQLite. Une panne physique de FRAM ne peut être résolue par ce plan et reste bloquante.

| Couple | Résultat exigé |
|---|---|
| Ancien firmware + partition historique préservée |boot historique habituel, LIVE invisible ; ses limites SD demeurent|
| Ancien firmware + record format 2 injecté dans ancien layout (cas négatif hors matériel) |refus unsupported/identité ; jamais permissif|
| Nouveau firmware + historique seul |lecture explicite, aucune conversion LIVE ; provisioning requis, pas d'auto-écriture|
| Nouveau firmware + LIVE valide et provenance conforme |config récupérée selon tuple, epoch autoritative ; acquisition arrêtée|
| Nouveau firmware + provenance ancienne modifiée |refus activation/réconciliation explicite|
| Ancien PC1 + firmware LIVE2 |incompatible avant commande, pas d'écriture|
| Nouveau PC + firmware 1 |décodeur V1 explicite, pas decommande 13/14 ni lecture 5028 comme V1|
| Nouveau PC + firmware 2 |contrôle B0/B4/B6 et epoch, lecture/commandes profil LIVE|

R0 spécifie ces garde-fous et cette allocation **comme norme de conception** ; la preuve matérielle de disponibilité de l'espace et la chaîne de migration ne sont pas encore acquises. Si la zone prévue ne peut être attribuée, R1 s'arrête avec les preuves : aucune réduction silencieuse des sauvegardes/banques historiques pour faire tenir l'implémentation.
