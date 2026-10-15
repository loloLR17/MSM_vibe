# Configuration persistante LIVE — tranche R1

Référence : mission `TR2-20261015-R1-LIVE-FRAM-CONFIGURATION` et norme
[`LIVE_R0`](../01_Specification_source/LIVE_R0/README.md). Ce document décrit
l'implémentation ; il ne modifie pas la norme ni les gels historiques.

## Portée et intégration

`LiveConfiguration` est une autorité de configuration utilisable hors acquisition,
sans port SD, capteur, campagne ou exécuteur de commandes. Son API locale ne vaut
pas admission B5. Le serveur de production reste composé en protocole 1 ; R1 n'y
raccorde ni B0 protocole 2 ni B4 LIVE. START_LIVE, STOP_LIVE, renouvellement d'epoch
et calcul B3 restent R2/R3. Aucun profil de production LIVE complet n'est annoncé.

- `configuration_validate_live` vérifie le tuple 2/0/0/durée 0, les domaines R0,
  IDs non nuls, labels ASCII, supervision 1 et warning <= alarm. Aucune capacité
  de campagne fictive n'est fournie au validateur historique.
- Les codecs configuration/identité format 2 sont explicites ; les décodeurs
  historiques refusent toujours ces formats. Les encodages de payload/CRC restent
  communs, contrôlés contre les trois vecteurs R0 inchangés.
- `live_configuration_project_b4` fournit une projection en mémoire schema 2,
  capacité validation LIVE 1, miroirs et CRC ; ce n'est pas un raccordement RTU.
- L'identité préparée conserve UID96, DeviceId, unit, série, fabricant et hardware,
  incrémente ses générations, utilise protocole 2 et conserve seulement les bits
  historiques 2/3. Ni acquisition, ni stockage, ni bit LIVE ne sont annoncés.
  R1 refuse une identité portant les capacités des moteurs futurs.

## Propriété et séquence d'utilisation

Allouer durablement les deux `TransactionalImageMedia`, `LiveConfiguration` et
une candidate de 52 298 octets, hors pile. Initialiser l'historique avec
`transactional_image_media_init_v2` et sa géométrie qualifiée ; initialiser LIVE
avec `transactional_image_media_init_live`. Les deux médias peuvent partager la
candidate : le service relit l'historique, capture sa provenance/slots, puis relit
LIVE. **Un seul propriétaire et aucune concurrence/réentrance** ; ne pas committer
un média depuis un autre service sans le récupérer à nouveau. Le registre
UID/DeviceId et le callback de propriété doivent vivre aussi longtemps que le
service. `acquisition_running` doit venir de l'état réel du futur ordonnanceur.

Le callback obligatoire de propriété reçoit la plage avant toute écriture
physique, et la plage complète avant préparation d'une mutation. Absent/refusé :
aucune écriture. Il appartient à l'intégrateur de prouver attribution exclusive,
cible, contenu et sauvegardes avant de le fournir ; ce callback ne découvre pas
ces faits. En R1 seul le modèle mémoire des tests dispose de cette autorisation.
Aucun fournisseur matériel de propriété ni provisioning automatique n'est ajouté.

`recover` est en lecture seule, sans création ni effet. `migrate` est une opération
explicite hors boot : elle exige un espace LIVE EMPTY et une identité historique
attribuée. Pas de formatage d'une racine inconnue. Elle copie les données
historiques puis crée une identité format 2, l'ancre epoch 1 et la provenance ;
le nouveau journal et BootIntent sont vides. Les originaux ne changent pas. Les
configurations campagne format 1 sont conservées, sans devenir une active LIVE.

`apply` refuse toute acquisition en cours avant I/O, contrôle CRC/domaines, relit
les autorités, écrit un record format 2 dans le slot opposé, puis engage l'image
complète avant de publier le snapshot actif. Les générations/révisions ne
bouclent pas. Une erreur après tentative d'écriture invalide l'autorité média
jusqu'à recovery ; une lecture défaillante après publication peut cacher un
commit pourtant durable, que le reboot récupère. Aucun faux succès B5 n'est émis.

## Recovery et rollback

LIVE possède les superblocks 122880/126976 et banques 131072/188416 (57344 chacune),
format logique 3/taille 52298. Toute écriture est bornée à [122880,245760) ; les
[0,122880) historiques et [245760,262144) réservés sont protégés.

Le sélecteur choisit la publication CRC-valide de génération maximale, puis
vérifie sa banque. Banque courante corrompue/incompatible : refus, sans retour
silencieux à une ancienne active. Une ancienne racine pointant une banque inactive
réécrite ne bloque pas la publication courante. Racines divergentes à génération
égale : conflit. Racine inconnue non vide : jamais EMPTY par défaut.

TR2M lie la copie au generation/SHA256 des 51914 octets historiques réels et à
l'UID96. Si l'historique change après rollback, le retour LIVE refuse l'activation
jusqu'à réconciliation explicite. Le lecteur historique retrouve son ancienne
partition inchangée. Le SHA256 portable suit [FIPS 180-4](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf) ; les tests de vecteurs ne sont pas une certification cryptographique.

L'ancre TR2Q epoch 1 valide est une **interface de réservation**, pas le moteur
transactionnel R2 : seul le record initial exact, sa seconde copie vide, les
réservations d'effet vides et le journal vide sont interprétés par R1. Tout état
futur est refusé, jamais rejoué. Le marqueur historique de slot endommagé peut
être conservé seulement à l'identique ; toute nouvelle corruption est refusée.

## Validation et ressources

Le test `live_configuration` utilise les vrais codecs, sélecteurs et commits,
avec un transport mémoire de 262144 octets seulement simulé. Il couvre les
vecteurs R0, 560 combinaisons de domaines, migration, refus, rollback/provenance,
corruptions, plafonds, erreurs de lecture aux barrières et coupures d'écriture.
Les 407 budgets par opération couvrent le payload tous les 256 octets puis chaque
octet à partir du budget 52224, incluant les headers/publications. Ce n'est pas
une exploration de chaque octet de payload ni une preuve d'atomicité électrique.
Après coupure d'activation, l'ancienne ou la nouvelle active complète doit rester
récupérable ; une migration interrompue peut exiger une réconciliation explicite.

Mesures Cortex-M33 GCC -O0 : service 576 octets, média 96 octets, candidate 52298.
Avec candidate partagée, total 53066 octets hors alignement, soit +1072 contre
l'ancien média V2 (80 octets) et sa candidate 51914. La structure média
partagée passe de 80 à 96 octets, y compris pour l'instance historique de
production (+16 par instance). Le service/candidate LIVE ne sont pas instanciés
dans la composition actuelle et leurs fonctions ne sont pas liées dans l'ELF. L'analyse `.su` mesure des cadres statiques,
pas la profondeur complète avec HAL/IRQ ni le high-water mark réel.

Un commit écrit payload 52298 + header 64 + publication 64 = **52426 octets**,
avec relectures/CRC synchrones. Ce coût n'est pas une latence mesurée sur carte.
Le futur moteur/ordonnanceur doit réserver STOP et journal et respecter les
budgets FRAM/FIFO/Modbus avant exploitation simultanée. R4 et R6 restent ouverts.
