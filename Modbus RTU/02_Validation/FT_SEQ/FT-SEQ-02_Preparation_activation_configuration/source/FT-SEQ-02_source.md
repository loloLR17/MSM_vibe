# FT-SEQ-02 — Référentiel source : préparation et activation d'une configuration

## 1. Objet

Identifier la chaîne séquentielle V1 réellement normative permettant à une centrale de préparer une configuration puis de la rendre active, sans reprendre les oracles unitaires déjà propriétaires de FT-BLK, FT-INT et FT-CMD.

## 2. Sources normatives et délégations

Sources V1 principales :
- `01_Specification_source/bloc4.md` ;
- `01_Specification_source/bloc5.md`.

Sources de validation gelées utilisées comme oracles délégués :
- `FT_BLK/FT-BLK-04_Cycle_configuration/` : séparation préparé/actif, cycle interne, CRC préparé et actif ;
- `FT_INT/FT-INT-02_Configuration_application/` : effets B5 → B4 après application réussie ;
- `FT_CMD/FT-CMD-05_Configuration_temps/` : acceptation/refus et résultat transactionnel APPLY CONFIG ;
- FT-STR / FT-ACC / FT-LIM pour structure, accès et domaines.

## 3. Chaîne normative retenue

La V1 établit les maillons suivants :
1. la centrale écrit/modifie l'image préparée B4 ;
2. après modification de `4B + 4C + 4D`, la centrale recalcule et écrit `prepared_config_crc` ;
3. la configuration préparée n'a aucun effet immédiat sur l'image active ;
4. l'activation passe obligatoirement par la commande B5 `APPLY CONFIG` ;
5. l'application n'est admise que lorsque les préconditions normatives sont satisfaites, notamment acquisition arrêtée, configuration préparée complète et valide, CRC conforme et `config_state = VALIDE` ;
6. après succès, la configuration devient active et l'image active B4 est cohérente avec la configuration appliquée.

Ces maillons sont déjà testés isolément par les familles propriétaires. La propriété FT-SEQ-02 est leur composition dans une seule chaîne fonctionnelle de bout en bout.

## 4. Exigences FT-SEQ-02

### SEQ02-R01 — Préparer puis activer une configuration valide

- Classification : `COVERED`.
- Propriétaire : FT-SEQ-02.
- Test : `TT-SEQ-CONFIG-001`.
- Exigence : une configuration préparée conformément aux exigences V1, laissée sans effet actif avant commande, puis soumise à une commande APPLY CONFIG réussie, doit aboutir à une configuration active correspondant à celle préparée.
- Nature de l'oracle FT-SEQ : continuité de la chaîne complète.
- Oracles élémentaires : délégués à FT-BLK-04, FT-CMD-05 et FT-INT-02.

### SEQ02-R02 — Une modification préparée ne constitue pas à elle seule une activation

- Classification : `DELEGATED`.
- Propriétaire : FT-BLK-04.
- Justification : invariant préparé/actif isolé ; FT-SEQ-02 l'utilise comme jalon de `TT-SEQ-CONFIG-001` mais ne le reteste pas comme exigence autonome.

### SEQ02-R03 — Recalcul et mise à jour du CRC préparé

- Classification : `DELEGATED`.
- Propriétaire : FT-BLK-04.
- Justification : calcul, sérialisation et vecteur CRC restent propriétaires du cycle de configuration.

### SEQ02-R04 — Acceptation et résultat de APPLY CONFIG

- Classification : `DELEGATED`.
- Propriétaire : FT-CMD-05.
- Justification : la décision transactionnelle B5 n'est pas redéfinie par FT-SEQ.

### SEQ02-R05 — Effets post-application sur B4

- Classification : `DELEGATED`.
- Propriétaire : FT-INT-02.
- Justification : `config_state = ACTIF`, identité active, image active et CRC actif sont des effets inter-blocs déjà couverts.

### SEQ02-R06 — Observation obligatoire d'un état intermédiaire VALIDE

- Classification : `NOT_DEFINED`.
- Justification : la logique V1 impose la validité préalable à l'application, mais ne définit ni durée de visibilité ni obligation qu'une centrale puisse observer `config_state = VALIDE` dans une transaction distincte entre validation et activation. FT-SEQ ne crée donc aucun oracle exigeant cette observation.

### SEQ02-R07 — Délai maximal de la chaîne préparation → activation

- Classification : `NOT_DEFINED`.
- Justification : aucune borne temporelle globale n'est définie par la V1.

### SEQ02-R08 — Incrément exact de config_revision_counter après activation

- Classification : `NOT_DEFINED`.
- Justification : la politique d'incrément n'est pas normée ; cette dette est déjà conservée par les familles gelées.

### SEQ02-R09 — Échec interne réel pendant l'application

- Classification : `CONDITIONAL`.
- Propriétaire élémentaire : FT-INT-02.
- Justification : la V1 prévoit `ERREUR_APPLICATION`, mais aucun mécanisme reproductible d'injection d'un échec interne n'est défini. FT-SEQ ne peut construire une séquence de reprise fiable sans moyen d'essai explicite.

## 5. Oracle composé de TT-SEQ-CONFIG-001

Le test séquentiel doit vérifier la continuité suivante :

`image active initiale` → `préparation B4 conforme` → `image active non modifiée par la seule préparation` → `APPLY CONFIG réussi` → `configuration active correspondant à la préparation`.

Le verdict FT-SEQ est PASS seulement si tous les jalons applicables réussissent dans la même exécution de scénario.

Un échec d'un oracle délégué entraîne l'échec du scénario, mais FT-SEQ ne réattribue pas la propriété de l'exigence élémentaire.

## 6. Anti-fabrication

Ne pas imposer :
- une séquence détaillée des états internes du moteur B5 ;
- une observation externe obligatoire de `VALIDE` ;
- un délai arbitraire entre préparation et activation ;
- un incrément particulier de `config_revision_counter` ;
- une politique de reprise après reboot ;
- une valeur ou un algorithme CRC différent de celui déjà gelé ;
- une règle issue uniquement des compléments métier informatifs.

## 7. Frontière avec FT-SEQ-07

Les scénarios `refus → correction de précondition → nouvelle commande → succès` sont réservés à FT-SEQ-07. FT-SEQ-02 couvre le chemin nominal complet et conserve seulement la traçabilité des refus vers FT-CMD-05.