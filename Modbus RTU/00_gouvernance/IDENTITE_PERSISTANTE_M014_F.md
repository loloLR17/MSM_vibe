# M014-F — layout interne V2 et identité persistante

Spécification avant codage, 11 octobre 2026. Décision M014-F : extension du seul
layout transactionnel, sans deuxième domaine physique ni changement Modbus V1.

## Allocation et bornes

Le préfixe historique [0,51818) conserve strictement tous les offsets de
system_persistent_layout.h : ConfigurationStore, TimeHistory, CampaignRepository,
réservation CampaignData historique même avec bulkSD, CommandJournal,
DiagnosticHistory/SelfTest et BootIntent [51802,51818). Aucun réemploi.
Identité ajoutée [51818,51914), 96 octets. Payload V1=51818, V2=51914.
Headers images et superblocks restent 64 octets, format physique1 ; format logique2.
Superblocks A[0,64), B[4096,4160). Zones images A[8192,65536),
B[65536,122880), chacune57344 octets ; header64+payload51914=51978,
reste5366 octets par zone. Fin122880 < capacité262144. Espaces entre régions et
[122880,262144) non alloués ici et non présumés libres sur la carte.
La validation de géométrie doit protéger les zones entières, pas seulement payload.
Le profil qualification existant est inchangé ; son ownership physique doit être
attesté avant écriture. Les allocations bulkSD restent indépendantes et exclusives.

## Enregistrement identité de 96 octets

Tous entiers big-endian, CRC32 IEEE réfléchi polynomial0xEDB88320,
initial/final XOR FFFFFFFF. Aucun padding C sérialisé.

| Offset | Taille | Champ |
|---|---:|---|
|0|4|magic TR2D|
|4|2|version codec1 (distincte de layout2 et protocole B0)|
|6|2|longueur96|
|8|8|génération identité non nulle, non UINT64_MAX|
|16|12|UID96, trois mots dans ordre HAL_GetUIDw0/w1/w2|
|28|4|DeviceId attribué explicitement|
|32|4|génération fonctionnelle IdentitySnapshot|
|36|12|hardware, firmware major/minor/patch, protocole, capacités (6 uint16)|
|48|16|serial ASCII, zéros autorisés pour remplissage|
|64|8|fabricant ASCII, zéros autorisés pour remplissage|
|72|1|adresse1..247 indépendante de DeviceId|
|73|19|réservés zéro|
|92|4|CRC32 des octets0..91|

L'inspection codec prouve intégrité/structure/UID/protocole/B0, pas unicité de parc
ni validité métier de toute configuration. Les services existants valident le
préfixe historique au boot ; EMPTY initial exige une image historique explicitement
fournie et validée par une autorité externe, jamais des zéros déclarés production.
L'UID matériel et toutes valeurs B0 doivent venir d'autorités réelles. Aucun
DeviceId, version, serial, fabricant ou adresse de production par défaut.

## Migration et sûreté

Lecteur V2 accepte V1 et V2, recovery sans écriture. L’API V1 du code M014-F refuse une
publication V2 plus récente dont le superblock est intègre, évitant un rollback
implicite. **Un ancien binaire antérieur à F ne bénéficie pas de cette protection** :
il peut reprendre un ancien V1 survivant. Tout downgrade production vers ce binaire
est interdit sans restauration dédiée autorisée et sauvegarde complète ; le
diagnosticE isolé ne monte pas ce média. Migration explicite
hors boot : génération attendue, ownership exclusif, autorisation écriture, UID96
exact, version protocole et attestation attribution nécessaires. Copier les51818
anciens octets inchangés, ajouter identité, écrire payload dans image inactive,
finaliser header puis publier superblock alternatif : un seul commit pour identité
et configuration. Ancienne publication conservée jusqu'au point de publication.
Coupure avant publication : V1 survivant ; après publication : V2 complet.
Pas format_empty dans migration. V2 identique avec génération courante : zéro write ;
identité différente/double provisioning/génération périmée : refus.
Création EMPTY seulement explicite, après recovery EMPTY et autorité du préfixe ;
publication image/header/superblock sans invalider des anciennes autorités.
Après coupure initiale avec header non publié : CORRUPTED, pas reformat automatique.
CRC n'est ni authentification cryptographique ni autorisation matérielle.

## Sauvegarde et restauration avant matériel

Identifier carte/ST-LINK/firmware diagnosticE ; sauvegarder/hash toute FRAM262144
et médias SD affectés avec ownership exclusif, conserver anciennes images et
manifeste UID/DeviceId/version/adresse/config/géométrie/génération/SHA. Vérifier
restaurabilité diagnosticE BIN/ELF et procédure standard qualifiée. Ne pas convertir
allow_flash en autorisation de formatage/restauration destructive inhabituelle.
Restaurer une sauvegarde matérielle uniquement sous procédure dédiée autorisée,
jamais boot ni retry silencieux. Après opération : recovery et comparaison complète.
Aucun HOTPLUG CubeProgrammer, aucun GDB pendant trafic, aucun replayC/I.

## Audit détaillé du préfixe et des autorités

| Domaine | Début | Taille | Fin exclusive | Organisation conservée |
|---|---:|---:|---:|---|
|Configuration|0|280|280|2 records140|
|TimeHistory|280|18|298|record18|
|CampaignRepository|298|4072|4370|2 allocator20 +8 campagnes ×2 records252|
|CampaignData historique|4370|11552|15922|4 slots :2 descriptors36 +32 chunks88|
|CommandJournal|15922|35840|51762|256 slots ×2 records70|
|DiagnosticHistory|51762|20|51782|record20|
|SelfTest|51782|20|51802|record20|
|BootIntent|51802|16|51818|record16|
|Identity codec1|51818|96|51914|enveloppe CRC explicite|

Calculs extraits des headers réels par programme C conservé horsGit ; static_assert
relie l’identité à la fin BootIntent. ConfigurationStore alterne ses deux records
à l’intérieur de la même image publiée : pas deuxième publication physique.
CampaignDataStorePersistent utilise toujours sa réservation historique ;
CampaignDataStoreBulk utilise un CampaignBulkMedia séparé, metadata8192octets
(8 slots ×2 descriptors512), blocs alignés512 et allocation dynamique validée
par scan_layout. Le SDMMC actuel expose le support brut entier : ownership doit
couvrir ce support, ses metadata et ses plages, pas seulement un fichier supposé.
Pas de conversion silencieuse FRAM→SD, pas de libération de la réserve11552.
L’initialisation SDMMC et les buffers/backendcontexts doivent être durables et
issus d’un média réel ; les instances de main.c restent dans le harness.

L’inspecteur d’identité est autoritatif pour son codec : il ne certifie pas tous
les records du préfixe. Le moteur exige validate_prefix fourni par une autorité
métier existante (formats/configuration/environnement réels), et attribution
fourni par un registre revu. Sans callback : refus, y compris création EMPTY.
Le vérificateur d’attribution fourni contrôle les doublons DeviceId et UID96,
la correspondance entière et l’absence d’entrée ; aucune entrée de production
n’est créée. Une liste de test n’est pas un registre de parc autoritatif.
Le manifeste réel devra versionner attribution UID96/DeviceId, fabricant/serial/
versions/capacités/adresse, origine et approbation, configuration et géométrie,
SHAs des images et outils, ownership, backup et génération attendue.

## Composition STM32 livrée et limites

Le runtime possède candidate51914, enregistrement et B0 durables. Avant boot
complet : ownership explicite et autorité protocole, FRAM réelle, recoverV2,
HAL_GetUIDw0/w1/w2 entiers, codec et comparaison UID/protocole. L’API du weak hook
ne peut plus injecter de B0 ni adresse ; elle ne fournit que transport qualifié
et dépendances. Adresse et B0 proviennent exclusivement de l’image persistée.
V1 doit être migré explicitement hors boot ; EMPTY/corrupt/unsupported/inaccessible
ne provoquent aucune création. Le boot complet existant valide les services et
peut écrire journaux/BootIntent : seule la phase probe/identité est lecture seule.

Le hook reste NOT_AVAILABLE : aucun registre d’attribution réel/valeurs B0/
configuration et manifeste de média exclusif trouvés, aucune image provisionnée
observée. L’autorité validate_prefix de production et les dépendances durables
FRAM/SPI/capteur/bulkSD/metadataallocator restent à raccorder sur ces données réelles.
Ce sont des préconditions concrètes de composition, pas un nouveau choix de layout.
Pas flash de harness par défaut ni fausse image production opérationnelle.
Le diagnosticE est conservé sur disque ; état MCU non relu dans cette mission.
Aucun nouveau COM7/RS485/debug/reset/flash/écriture FRAM ou SD réel.
M014 reste IN PROGRESS : polling physique et perte/reprise non démontrés.
