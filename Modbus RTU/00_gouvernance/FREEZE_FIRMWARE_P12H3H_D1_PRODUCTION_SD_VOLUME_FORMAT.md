# Gel P12 / H3h — D1 — Format et géométrie du volume microSD de production

## 1. Statut et références

**CONTRAT NORMATIF GELÉ — DOCUMENTAIRE UNIQUEMENT.**

Baseline examinée : `9ff3ccd083e86f06e1a57e61aaf0401390290569`.
Les décisions D1 ci-dessous sont approuvées ; elles ne sont pas présentées
comme implémentées ou qualifiées. D1 précise les choix laissés ouverts par D0.
La séquence D1/D2/D3 concerne la composition production, sans renommer les
sous-tranches historiques H3h ou le format campagne D4-C.

Références normatives :

- [Composition A](FREEZE_FIRMWARE_P12H3H_PRODUCTION_CAMPAIGN_STORAGE_COMPOSITION_A.md) ;
- [Architecture D0](FREEZE_FIRMWARE_P12H3H_D0_PRODUCTION_SD_VOLUME_READINESS.md) ;
- [Format campagne D4-C](CONCEPTION_FIRMWARE_P12H3H_D4C_MULTI_CAMPAIGN_BULK_LAYOUT.md) ;
- [Procédure E4](PROCEDURE_FIRMWARE_P12H3H_E4_PHYSICAL_POWERLOSS_RECOVERY.md).

## 2. Faits existants du repository

Sources sous `../05_Firmware/` :

- `include/tr2/persistence/campaign_bulk_media.h` : interface octet/capacité
  uint64 et read/write/sync, sans identité volume.
- `include/tr2/persistence/campaign_data_store_bulk.h` et
  `include/tr2/persistence/campaign_repository_store.h` : huit slots, deux
  descriptors de 512 octets par slot, soit 8192 octets de metadata.
- `include/tr2/persistence/campaign_bulk_metadata.h` et
  `src/persistence/campaign_bulk_metadata.c` : descriptors A/B, encodage
  numérique big-endian, génération, CRC et préfixe durable.
- `include/tr2/persistence/campaign_bulk_block.h` et
  `src/persistence/campaign_bulk_block_stream.c` : header de 32 octets,
  trailer de 4 octets, records de 16 octets, extents arrondis au secteur.
- `platform/stm32/stm32_sdmmc_bulk_media.c` : capacité obtenue par
  LogBlockNbr × LogBlockSize, taille logique attendue de 512 octets.
- `include/tr2/persistence/system_persistent_layout.h` : réservation data
  historique et offsets persistants conservés.

Ces mécanismes existants ne constituent pas une implémentation du volume D1.

## 3. Layout V1 — gelé

La microSD est dédiée au TR2. Le volume V1 commence à l'offset physique 0.

```text
microSD / volume TR2 V1
+0       : Volume Header A — 512 octets
+512     : Volume Header B — 512 octets
+1024    : début de la région CampaignDataStoreBulk
           longueur explicitement bornée : bulk_length_bytes
```

Les headers sont extérieurs au backend. CampaignDataStoreBulk reçoit
uniquement une fenêtre définie par bulk_offset_bytes et bulk_length_bytes ;
les offsets D4-C sont relatifs à cette fenêtre.

En V1, bulk_offset_bytes = 1024. La longueur bulk est positive et sa région
reste dans le volume. **L'égalité bulk_length_bytes = volume_length_bytes -
1024 n'est pas gelée.** Le provisioning pourra utiliser toute la partie
restante, mais ce choix n'est pas une propriété du format.
Aucun offset ou taille de fixture E1/E3/E4 n'est réutilisé en production.

## 4. Header V1 — gelé

Chaque copie occupe exactement un secteur logique de 512 octets.
Les entiers sont encodés big-endian, sans dépendre du padding d'une structure
C. Les identités sont des séquences opaques de 16 octets.

| Offset dans la copie | Taille | Champ | Contrat V1 |
|---:|---:|---|---|
| 0 | 4 | magic | 0x54523256, octets ASCII TR2V |
| 4 | 2 | format_version | 1 |
| 6 | 2 | record_length | 512 |
| 8 | 8 | generation | uint64 non nul, sans wraparound |
| 16 | 16 | volume_identity | Identité logique volume |
| 32 | 16 | owner_tr2_identity | Identité logique propriétaire |
| 48 | 8 | volume_length_bytes | Longueur du volume |
| 56 | 8 | bulk_offset_bytes | 1024 |
| 64 | 8 | bulk_length_bytes | Longueur de la fenêtre bulk |
| 72 | 436 | reserved | Tous les octets zéro ; vérification obligatoire |
| 508 | 4 | crc32 | CRC des octets 0 à 507 inclus, stocké big-endian |

Les champs couvrent exactement les octets 0 à 511, sans trou ni recouvrement :
4 + 2 + 2 + 8 + 16 + 16 + 8 + 8 + 8 + 436 + 4 = 512.

La magic et la version sont désormais figées. La recherche des signatures
existantes dans les sources a identifié notamment 0x5452324D, 0x54523249,
0x54523254, 0x54523242, 0x54524448, 0x54525354, 0x54523243, 0x5452324A,
0x54523241, 0x54523244, 0x5452324B, 0x5452424D, 0x54524244 et 0x45344632 ;
aucune ne vaut 0x54523256. Ce choix n'ajoute aucune autre sémantique.

## 5. Identités et association — gelées

Le TR2 possède une identité LOGIQUE de 128 bits, indépendante de l'UID
STM32. Elle est destinée à survivre au remplacement du MCU/carte CPU lorsque
sa persistance est conservée ou transférée explicitement. L'UID pourra
contribuer ultérieurement à la génération ou au diagnostic, sans devenir
l'identité fonctionnelle du TR2.

Chaque volume possède une identité logique propre de 128 bits distincte,
créée explicitement au provisioning. Un reprovisionnement destructif crée
une nouvelle identité volume. Cette identité n'est déduite ni du campaign_id,
ni de l'UID MCU, ni de la capacité, ni des metadata D4-C, ni du contenu
arbitraire du média.

La source et la méthode fiables de génération des identités ne sont pas
gelées ; elles doivent être définies avant implémentation du provisioning
et de l'association en D3.

owner_tr2_identity contient l'identité logique du propriétaire. Un volume
valide d'un autre TR2 est étranger et n'est jamais adopté automatiquement.
Le futur domaine persistant transactionnel côté TR2 conserve au minimum :

- TR2 logical identity ;
- associated volume identity ;
- accepted volume-header generation.

D1 ne définit aucun offset ni format FRAM. Les autorités FRAM et microSD
restent distinctes, sans transaction atomique commune implicite.

## 6. Génération et publication A/B — gelées

La génération est un uint64 non nul ; zéro est réservé et invalide.
Pas de wraparound. Toute publication ultérieure utilise une génération
strictement supérieure. À la génération maximale, une opération nécessitant
une nouvelle publication est refusée, sans retour silencieux à zéro.
La génération ordonne les publications et assure la cohérence accidentelle ;
elle n'est pas un mécanisme cryptographique anti-rollback.

Le volume possède exactement deux copies sectorielles A/B.
Publication conceptuelle d'une nouvelle autorité :

1. conserver l'autorité existante ;
2. écrire la copie inactive avec la génération suivante ;
3. sync ;
4. relire ;
5. vérifier intégralement la copie, y compris sa correspondance avec le
   contenu attendu ;
6. seulement ensuite la considérer publiée.

Le protocole opérationnel complet reste D3, notamment première publication,
reprovisionnement et association. A/B protège contre certaines écritures
interrompues, sans garantie d'indépendance des erase units internes de la SD.

## 7. Validation et sélection de l'autorité — gelées

Une copie valide satisfait le format V1, le CRC, les réservés, la génération
et les contrôles de géométrie. La sélection d'une autorité ne suffit pas à
rendre le stockage ready ni à autoriser START.

| Situation | Règle |
|---|---|
| Deux copies valides, même génération, contenu identique | Autorité unique acceptable, sous contrôle de l'association |
| Deux copies valides, même génération, contenu différent | Ambiguïté : refus |
| Deux copies valides, générations différentes | Génération supérieure sélectionnable seulement si les propriétés immuables nécessaires sont cohérentes |
| Identité volume ou géométrie incompatible entre copies valides | Refus ; aucun choix aveugle par génération |
| Une copie valide, autre structurellement invalide/corrompue | Acceptation dégradée seulement sous les trois correspondances exactes ci-dessous |
| Une copie impossible à lire | Refus d'utilisation ; une autorité plus récente pourrait être inconnue |
| Une copie portant une version non supportée | Refus ; aucun repli silencieux sur une ancienne version connue |
| Deux copies vierges | Volume non provisionné ; aucun provisioning automatique |
| Aucune copie valide | Refus ; aucune réparation automatique au boot |

Entre copies valides de générations différentes, les invariants nécessaires
sont l'identité volume et la géométrie : volume_length_bytes,
bulk_offset_bytes, bulk_length_bytes. Un changement de propriétaire relève
du protocole explicite D3 ; il ne permet pas une adoption au boot.

L'acceptation dégradée exige simultanément :

- volume_identity exactement égale au volume associé persisté ;
- owner_tr2_identity exactement égale à l'identité logique du TR2 ;
- generation exactement égale à la génération acceptée côté TR2.

Toute sélection reste soumise au contrôle de l'association persistée.
Ne pas préférer une ancienne copie parce que la nouvelle ne correspond pas
à l'association attendue. La classification et les priorités diagnostiques
des cas combinés, ainsi que la reconnaissance du contenu vierge, restent D2 ;
elles ne doivent pas affaiblir les refus ci-dessus.

## 8. Version et CRC — gelés

Une version inconnue n'est jamais interprétée comme EMPTY. Une magic inconnue
n'est jamais interprétée comme volume TR2 valide. record_length doit valoir
512 et tous les reserved doivent être zéro. Un CRC invalide signifie copie
structurellement invalide, sous réserve des classifications détaillées D2.
Aucun mécanisme de migration automatique n'est créé.

Le CRC reprend exactement crc32_bytes() dans
`../05_Firmware/src/persistence/campaign_bulk_metadata.c` :

- initialisation 0xFFFFFFFF ;
- traitement réfléchi bit par bit, polynôme 0xEDB88320 ;
- XOR final 0xFFFFFFFF ;
- couverture volume : exactement 508 octets, offsets 0 à 507 inclus ;
- résultat uint32 encodé big-endian à l'offset 508.

Le CRC protège contre les corruptions accidentelles. Il ne fournit ni
authenticité, ni protection contre clonage, ni sécurité cryptographique.

## 9. Géométrie et borne structurelle — gelées

Toutes les grandeurs sont en octets :

```text
P = capacité physique microSD
V = 0, base physique du volume V1
L = volume_length_bytes
H = 1024, overhead des deux headers
B = bulk_offset_bytes = 1024
C = bulk_length_bytes
```

Contrôles minimaux avant utilisation :

```text
P connu et compatible avec le média
L multiple de 512
L <= P
B = 1024
B multiple de 512
C > 0
C multiple de 512
B <= L
C <= L - B
```

Vérifier B <= L avant soustraction ; utiliser C <= L - B plutôt qu'une
addition B + C susceptible de déborder. L peut être inférieur à P par choix
explicite du provisioning. La partie éventuelle après B+C n'appartient pas
au backend bulk ; aucun usage de cette partie n'est spécifié ici.

D4-C impose 8 slots × 2 descriptors × 512 = 8192 octets de metadata.
Le premier extent physique minimal nécessite 512 octets. Donc :

```text
C >= 8192 + 512 = 8704 octets
L >= 1024 + 8704 = 9728 octets
```

**CECI EST UNE BORNE STRUCTURELLE DU FORMAT, PAS UNE TAILLE DE CARTE OU
DE VOLUME RECOMMANDÉE EN PRODUCTION.**

Les contraintes de profil de blocs, buffers et recovery restent à prendre
en compte avant qualification production. Capacité physique, longueur volume,
overhead volume, région bulk, overhead D4-C, espace durable consommé et espace
encore disponible restent distincts. La capacité brute n'est ni une capacité
logique utile exacte ni une garantie d'admission.

storage_limit_mb reste un plafond logique par campagne ; MB décimal signifie
1 000 000 octets. Réservation, admission et comportement à atteinte du plafond
restent E. Aucune formule définitive de capacité publiable n'est ajoutée ici.

## 10. Power-loss et absence de PREPARING/READY

| Situation | Conséquence conceptuelle |
|---|---|
| Avant toute première publication valide | Volume non utilisable |
| Pendant écriture inactive | Ancienne autorité éventuellement conservée, sous contrôle strict de l'association persistée |
| Nouvelle copie durable, FRAM non mise à jour | Incohérence : refus jusqu'à résolution D3 |
| FRAM mise à jour, nouvelle copie illisible | Ne pas accepter automatiquement l'ancienne génération |
| Deux copies incompatibles | Refus |
| Boot normal | Aucune réparation persistante |

La transaction FRAM/microSD n'est pas atomique. D3 doit concevoir le protocole
et les états intermédiaires, y compris les anciennes autorités restant après
reprovisionnement interrompu. Aucun automatisme de résolution n'est déduit.

Le header V1 n'a aucun champ PREPARING/READY. D3 devra démontrer la sûreté
face aux interruptions des opérations de préparation, publication et
association. Si un état persistant supplémentaire est indispensable, D1 doit
être explicitement rouvert et amendé avant implémentation. Aucun champ n'est
ajouté préventivement.

## 11. Décisions différées et non-objectifs

| Tranche | Responsabilités différées |
|---|---|
| D2 | Readiness détaillée, priorités/diagnostics, représentation interne puis exposition éventuelle |
| D3 | Provisioning, génération fiable des identités, association/réassociation, protocole FRAM/microSD, remplacement et power-loss opérationnel |
| D4 production | Composition STM32 et intégration SystemRuntime avec fenêtre bornée |
| D5 production | Validation logicielle puis qualification matérielle |
| E | Quota/admission storage_limit_mb |

Les longueurs numériques du volume et de la région bulk seront choisies
explicitement dans les bornes gelées. D1 ne définit aucun emplacement FRAM,
commande Modbus ou interface opérationnelle de provisioning.

Le header ne contient ni catalogue de campagnes, timestamp, nom texte,
paramètres acquisition, storage_limit_mb, quota, état campagne, informations
récupérables de D4-C ou mécanisme cryptographique.

D1 ne modifie ni D4-C, ni E4, ni CampaignBulkMedia, ni les formats/offsets FRAM,
ni SystemRuntime. La réservation data historique reste conservée. Aucun
fallback, reprise OPEN, formatage, nettoyage, réparation, adoption, reclaim
ou rétention implicite au boot n'est introduit. Les garanties synchrones du
gel A sont conservées ; FIFO/DMA, acquisition, buffering et stalls restent
hors périmètre.

## 12. E4 et validation documentaire

E1/E2/E3 conservent leur portée documentée ; D1 ne les étend pas au volume.
E4 physique reste pending, attachée exactement à la baseline firmware
`111ca804c08cd126abc0ae10ff2a923ce7554ebe`, à reconstruire pour qualification.
Le développement continue sur main ; l'applicabilité de sa future preuve
au firmware production sera évaluée séparément. D1 ne modifie pas le harness.

La validation de ce gel porte sur la conformité aux arbitrages D1-F,
la cohérence A/D0, le layout octet par octet, les bornes sans overflow,
l'absence de fixture promue en constante production, l'inspection du diff
complet et git diff --check.

Aucun firmware, test, CMake ou format existant n'est modifié. Une compilation
n'apporterait aucune preuve supplémentaire à cette tranche documentaire ;
elle n'est pas requise. Aucun résultat d'implémentation ou matériel n'est
annoncé.
