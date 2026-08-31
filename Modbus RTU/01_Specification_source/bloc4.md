# TR2 --- Bloc 4 : Configuration acquisition

## Mapping Modbus RTU --- V1

------------------------------------------------------------------------

## 1. Objet du bloc

Le Bloc 4 permet à la centrale de :

-   préparer une configuration d'acquisition ;
-   configurer la supervision vibratoire ;
-   définir le contexte de campagne ;
-   activer une configuration validée.

Ce bloc repose sur une logique **préparé / validé / actif**, robuste et
traçable.

------------------------------------------------------------------------

## 2. Base d'adressage

-   **Bloc 4 base = registre 4000**

------------------------------------------------------------------------

## 3. Accès

  Zone                                  Accès
  ------------------------------------- -----------
  Métadonnées (4A)                      mixte
  Configuration préparée (4B, 4C, 4D)   RW
  Image active (4E)                     RO strict

------------------------------------------------------------------------

## 4. Conventions générales

-   Types autorisés : `uint16`, `uint32`, `enum16`, `bitfield16`,
    `ASCII fixe`
-   `uint32` = **MSW puis LSW**
-   Chaînes ASCII :
    -   longueur fixe (32 caractères)
    -   padding `0x00`
-   Registres réservés :
    -   lecture = `0`
    -   écriture interdite
-   Tous les champs doivent être exposés de manière **cohérente lors
    d'une même lecture**
-   ID = `0` → non renseigné (interdit pour validation)

------------------------------------------------------------------------

## 5. Convention normative V1 des seuils vibratoires

En version V1, tous les seuils de supervision vibratoire du Bloc 4 sont
définis sur des **accélérations exprimées en mg**.

Cela concerne :

-   `rms_warn_threshold_mg`
-   `rms_alarm_threshold_mg`
-   `peak_warn_threshold_mg`
-   `peak_alarm_threshold_mg`
-   `threshold_hysteresis_mg`

Ces seuils s'appliquent directement aux indicateurs du Bloc 3, sans
conversion d'unité.

------------------------------------------------------------------------

## 6. CRC de configuration

### 6.1 Algorithme

CRC-32 / IEEE 802.3 :

-   width = 32
-   poly = 0x04C11DB7
-   init = 0xFFFFFFFF
-   refin = true
-   refout = true
-   xorout = 0xFFFFFFFF
-   check = 0xCBF43926

------------------------------------------------------------------------

### 6.2 Périmètre

-   `prepared_config_crc` → offsets 16 à 99
-   `active_config_crc` → offsets 100 à 175

------------------------------------------------------------------------

### 6.3 Sérialisation

-   ordre croissant des registres
-   chaque registre :
    -   MSB puis LSB
-   `uint32` :
    -   MSW puis LSW
-   ASCII et réservés inclus tels quels

------------------------------------------------------------------------

### 6.4 Exemple de vecteur de test CRC

Afin de permettre la validation croisée firmware / centrale /
simulateur, au moins un vecteur de test CRC doit être figé dans la
documentation projet.

Exemple de principe : - charger un jeu de registres de référence sur la
zone `4B + 4C + 4D`, - sérialiser les registres dans l'ordre
croissant, - injecter chaque registre en ordre octet fort puis octet
faible, - calculer le CRC-32/IEEE 802.3, - comparer à la valeur
attendue.

La valeur attendue du vecteur de test de référence doit être figée dans
les annexes de validation ou dans les jeux de tests automatisés.

#### Vecteur de test normatif n°1 --- zone préparée

Jeu de données injecté sur les offsets `16` à `99` du Bloc 4 :

-   tous les registres non explicitement listés ci-dessous valent
    `0x0000`
-   `4016 = 0x682B` (`sampling_frequency_hz = 26667`)
-   `4018 = 0x0007` (`axes_enable_mask = X+Y+Z`)
-   `4019 = 0x0002` (`full_scale_code`)
-   `4020 = 0x0001` (`acquisition_mode`)
-   `4021 = 0x8000` (`window_size_samples = 32768`)
-   `4022 = 0x1388` (`indicator_period_ms = 5000`)
-   `4023 = 0x0000`
-   `4024 = 0x0E10` (`campaign_duration_s = 3600`)
-   `4025 = 0x0001` (`storage_mode`)
-   `4026 = 0x0000`
-   `4027 = 0x0200` (`storage_limit_mb = 512`)
-   `4040 = 0x0001` (`supervision_enable_mask`)
-   `4041 = 0x0064` (`rms_warn_threshold_mg = 100`)
-   `4042 = 0x00C8` (`rms_alarm_threshold_mg = 200`)
-   `4043 = 0x012C` (`peak_warn_threshold_mg = 300`)
-   `4044 = 0x0190` (`peak_alarm_threshold_mg = 400`)
-   `4045 = 0x0014` (`threshold_hysteresis_mg = 20`)
-   `4046 = 0x01F4` (`alarm_hold_time_ms = 500`)
-   `4056 = 0x0000`
-   `4057 = 0x0001` (`campaign_context_id = 1`)
-   `4058 = 0x0000`
-   `4059 = 0x0002` (`mission_id = 2`)
-   `4092 = 0x0001` (`operating_mode_code`)
-   `4093 = 0x0002` (`navigation_zone_code`)
-   `4094 = 0x0003` (`load_state_code`)
-   `4095 = 0x0001` (`sea_state_code`)

#### Valeur attendue du vecteur de test normatif n°1

Pour le jeu de registres de référence défini ci-dessus, la valeur CRC
attendue est :

-   **`0x5207CCFC`**

Cette valeur constitue la référence normative de validation croisée : -
firmware, - centrale, - simulateur, - tests automatisés.

#### Règle normative

Aucune implémentation firmware ou centrale ne peut être considérée
validée tant que ce vecteur de test CRC n'est pas passé avec la valeur
attendue figée.

------------------------------------------------------------------------

## 7. États de configuration

  Code      État
  --------- --------------------
  0         VIDE
  1         BROUILLON
  2         VALIDE
  3         RÉSERVÉ
  4         ACTIF
  5         ERREUR_VALIDATION
  6         ERREUR_APPLICATION
  7..65535  RÉSERVÉ

------------------------------------------------------------------------

Règles :
- la valeur `3` est réservée et ne doit pas être produite par une implémentation V1 ;
- toute valeur réservée reçue ou observée doit être considérée comme non supportée par la centrale ;
- aucune signification implicite ne doit être attribuée à une valeur réservée.

### 7.1 Transitions autorisées

Les transitions d'état du champ `config_state` doivent respecter la
logique suivante :

  -----------------------------------------------------------------------
  État courant            Événement               État suivant
  ----------------------- ----------------------- -----------------------
  `VIDE`                  écriture partielle ou   `BROUILLON`
                          initiale d'une          
                          configuration           

  `BROUILLON`             validation réussie      `VALIDE`

  `BROUILLON`             validation échouée      `ERREUR_VALIDATION`

  `ERREUR_VALIDATION`     modification d'au moins `BROUILLON`
                          un champ préparé        

  `VALIDE`                commande d'application  `ACTIF`
                          réussie                 

  `VALIDE`                commande d'application  `ERREUR_APPLICATION`
                          échouée                 

  `ERREUR_APPLICATION`    modification d'au moins `BROUILLON`
                          un champ préparé        

  `ACTIF`                 préparation d'une       `BROUILLON`
                          nouvelle configuration  
                          distincte               
  -----------------------------------------------------------------------

#### Règles

-   Une configuration ne peut pas passer directement de `VIDE` à
    `ACTIF`.
-   Une configuration ne peut pas être appliquée si elle n'est pas en
    état `VALIDE`.
-   Toute modification d'un champ de la zone préparée invalide
    implicitement l'état `VALIDE` précédent et repositionne la
    configuration en `BROUILLON`.

------------------------------------------------------------------------

## 8. Mapping complet

  --------------------------------------------------------------------------------------------
    Offset      Adresse Champ                      Type          Accès   Description
                absolue                                                  
  -------- ------------ -------------------------- ------------- ------- ---------------------
         0         4000 config_structure_version   uint16        RO      Version structure
                                                                         Bloc 4

         1         4001 config_capabilities_mask   uint16        RO      Masque de capacités

       2-3    4002-4003 prepared_config_id         uint32        RW      ID configuration
                                                                         préparée

       4-5    4004-4005 active_config_id           uint32        RO      ID configuration
                                                                         active

         6         4006 config_state               uint16        RO      État configuration

         7         4007 config_error_code          uint16        RO      Code erreur
                                                                         configuration

       8-9    4008-4009 prepared_config_crc        uint32        RW      CRC configuration
                                                                         préparée

     10-11    4010-4011 active_config_crc          uint32        RO      CRC configuration
                                                                         active

     12-13    4012-4013 config_revision_counter    uint32        RO      Compteur de révision

     14-15    4014-4015 reserved_4A                uint16\[2\]   RO      Réservé
  --------------------------------------------------------------------------------------------

------------------------------------------------------------------------

### 8.1 Zone 4B --- Acquisition préparée

  --------------------------------------------------------------------------------------------
    Offset     Adresse Champ                   Type           Accès   Description
  -------- ----------- ----------------------- -------------- ------- ------------------------
        16        4016 sampling_frequency_hz   uint16         RW      Fréquence
                                                                      d'échantillonnage

        17        4017 reserved_4B_0           uint16         RO      Réservé

        18        4018 axes_enable_mask        bitfield16     RW      Activation axes

        19        4019 full_scale_code         enum16         RW      Pleine échelle

        20        4020 acquisition_mode        enum16         RW      Mode acquisition

        21        4021 window_size_samples     uint16         RW      Taille fenêtre

        22        4022 indicator_period_ms     uint16         RW      Période indicateurs

     23-24   4023-4024 campaign_duration_s     uint32         RW      Durée campagne

        25        4025 storage_mode            enum16         RW      Mode stockage

     26-27   4026-4027 storage_limit_mb        uint32         RW      Limite stockage

     28-39   4028-4039 reserved_4B             uint16\[12\]   RO      Réservé
  --------------------------------------------------------------------------------------------

#### 8.1.1 Domaines normatifs V1 --- Acquisition préparée

Les champs de la zone 4B doivent respecter les domaines suivants pour
qu'une configuration préparée puisse être validée :

  -----------------------------------------------------------------------
  Champ                               Domaine V1
  ----------------------------------- -----------------------------------
  `sampling_frequency_hz`             `26667` uniquement

  `axes_enable_mask`                  bit 0 = X ; bit 1 = Y ; bit 2 = Z ;
                                      bits 3 à 15 réservés à `0` ;
                                      valeurs valides `0x0001` à `0x0007`

  `full_scale_code`                   `0` = ±2 g ; `1` = ±4 g ; `2` = ±8
                                      g ; `3` = ±16 g ; `4..65535`
                                      réservés / invalides en V1

  `acquisition_mode`                  `0` = non configuré / aucun mode,
                                      invalide pour une configuration
                                      validée ; `1` = acquisition de
                                      campagne standard ; `2..65535`
                                      réservés / invalides en V1

  `window_size_samples`               `4096`, `8192`, `16384` ou `32768`

  `indicator_period_ms`               `2000`, `5000`, `10000`, `30000` ou
                                      `60000` ms

  `campaign_duration_s`               `60..604800` s inclus

  `storage_mode`                      `0` = non configuré / aucun
                                      stockage, invalide pour une
                                      configuration validée ; `1` =
                                      stockage local de campagne en mode
                                      store-and-forward ; `2..65535`
                                      réservés / invalides en V1

  `storage_limit_mb`                  `1..4294967295` MB au niveau
                                      représentation ; la valeur doit
                                      être compatible avec la capacité de
                                      stockage utilisable déterminée par
                                      le firmware
  -----------------------------------------------------------------------

Règles complémentaires :

-   `axes_enable_mask = 0x0000` est invalide : au moins un axe doit être
    actif ;
-   `indicator_period_ms` doit être supérieur ou égal à la durée de la
    fenêtre d'acquisition :
    `1000 × window_size_samples / sampling_frequency_hz` ;
-   `storage_limit_mb = 0` est invalide ;
-   une valeur hors domaine écrite dans un champ RW de la zone préparée
    ne constitue pas une erreur d'adressage Modbus : l'écriture est
    acceptée dans la zone préparée, mais la validation de la
    configuration doit échouer et son application est interdite ;
-   une écriture sur un registre RO ou réservé reste interdite
    conformément aux règles générales d'accès.

------------------------------------------------------------------------

### 8.2 Zone 4C --- Supervision préparée

  ----------------------------------------------------------------------------------------
    Offset     Adresse Champ                     Type          Accès   Description
  -------- ----------- ------------------------- ------------- ------- -------------------
        40        4040 supervision_enable_mask   uint16        RW      Masque supervision

        41        4041 rms_warn_threshold_mg     uint16        RW      Seuil RMS warning

        42        4042 rms_alarm_threshold_mg    uint16        RW      Seuil RMS alarme

        43        4043 peak_warn_threshold_mg    uint16        RW      Seuil crête warning

        44        4044 peak_alarm_threshold_mg   uint16        RW      Seuil crête alarme

        45        4045 threshold_hysteresis_mg   uint16        RW      Hystérésis

        46        4046 alarm_hold_time_ms        uint16        RW      Maintien alarme

     47-55   4047-4055 reserved_4C               uint16\[9\]   RO      Réservé
  ----------------------------------------------------------------------------------------

------------------------------------------------------------------------

### 8.3 Zone 4D --- Contexte campagne

  ----------------------------------------------------------------------------------
    Offset     Adresse Champ                  Type          Accès   Description
  -------- ----------- ---------------------- ------------- ------- ----------------
     56-57   4056-4057 campaign_context_id    uint32        RW      ID contexte

     58-59   4058-4059 mission_id             uint32        RW      ID mission

     60-75   4060-4075 campaign_label         ASCII fixe    RW      Label campagne

     76-91   4076-4091 mission_label          ASCII fixe    RW      Label mission

        92        4092 operating_mode_code    uint16        RW      Mode opératoire

        93        4093 navigation_zone_code   uint16        RW      Zone navigation

        94        4094 load_state_code        uint16        RW      État charge

        95        4095 sea_state_code         uint16        RW      État mer

     96-99   4096-4099 reserved_4D            uint16\[4\]   RO      Réservé
  ----------------------------------------------------------------------------------

------------------------------------------------------------------------

### 8.4 Zone 4E --- Image active compacte (RO)

La zone 4E expose une image active compacte et normalisée de la
configuration appliquée.

Règles : - les champs métier appliqués issus des zones 4B, 4C et 4D y
sont exposés en lecture seule ; - l'ordre et le regroupement des champs
réservés peuvent différer des zones préparées ; - tous les registres
réservés de 4E doivent lire `0` ; - la zone 4E constitue la référence
pour `active_config_crc`.

  -------------------------------------------------------------------------------------------
     Offset     Adresse Champ                            Type          Accès   Description
  --------- ----------- -------------------------------- ------------- ------- --------------
        100        4100 active_sampling_frequency_hz     uint16        RO      Miroir actif

        101        4101 active_axes_enable_mask          bitfield16    RO      Miroir actif

        102        4102 active_full_scale_code           enum16        RO      Miroir actif

        103        4103 active_acquisition_mode          enum16        RO      Miroir actif

        104        4104 active_window_size_samples       uint16        RO      Miroir actif

        105        4105 active_indicator_period_ms       uint16        RO      Miroir actif

    106-107   4106-4107 active_campaign_duration_s       uint32        RO      Miroir actif

        108        4108 active_storage_mode              enum16        RO      Miroir actif

    109-110   4109-4110 active_storage_limit_mb          uint32        RO      Miroir actif

    111-115   4111-4115 reserved_4E_A                    uint16\[5\]   RO      Réservé, doit
                                                                               lire 0

        116        4116 active_supervision_enable_mask   uint16        RO      Miroir actif

        117        4117 active_rms_warn_threshold_mg     uint16        RO      Miroir actif

        118        4118 active_rms_alarm_threshold_mg    uint16        RO      Miroir actif

        119        4119 active_peak_warn_threshold_mg    uint16        RO      Miroir actif

        120        4120 active_peak_alarm_threshold_mg   uint16        RO      Miroir actif

        121        4121 active_threshold_hysteresis_mg   uint16        RO      Miroir actif

        122        4122 active_alarm_hold_time_ms        uint16        RO      Miroir actif

    123-127   4123-4127 reserved_4E_B                    uint16\[5\]   RO      Réservé, doit
                                                                               lire 0

    128-129   4128-4129 active_campaign_context_id       uint32        RO      Miroir actif

    130-131   4130-4131 active_mission_id                uint32        RO      Miroir actif

    132-147   4132-4147 active_campaign_label            ASCII fixe    RO      Miroir actif,
                                                                               ASCII fixe 32
                                                                               caractères,
                                                                               padding 0x00

    148-163   4148-4163 active_mission_label             ASCII fixe    RO      Miroir actif,
                                                                               ASCII fixe 32
                                                                               caractères,
                                                                               padding 0x00

        164        4164 active_operating_mode_code       uint16        RO      Miroir actif

        165        4165 active_navigation_zone_code      uint16        RO      Miroir actif

        166        4166 active_load_state_code           uint16        RO      Miroir actif

        167        4167 active_sea_state_code            uint16        RO      Miroir actif

    168-175   4168-4175 reserved_4E_C                    uint16\[8\]   RO      Réservé, doit
                                                                               lire 0
  -------------------------------------------------------------------------------------------

------------------------------------------------------------------------

## 9. Règles d'implémentation

-   La configuration préparée n'a **aucun effet immédiat**
-   L'activation passe obligatoirement par :
    -   **Bloc 5 --- commande "appliquer configuration"**
-   L'image active doit être :
    -   cohérente
    -   figée
-   Après toute modification de la zone préparée `4B + 4C + 4D`, la centrale doit recalculer le CRC et mettre à jour `prepared_config_crc`.
-   Lors de la commande d’application, le firmware doit recalculer indépendamment le CRC de la zone préparée et le comparer à `prepared_config_crc`.
-   `active_config_crc` est calculé par le firmware à partir de l’image active `4E` et mis à jour lors de toute modification de cette image active.
-   Les zones réservées doivent rester à 0

------------------------------------------------------------------------

## 10. Justification

Ce design permet :

-   robustesse face aux erreurs Modbus ;
-   validation avant activation ;
-   traçabilité complète des configurations ;
-   cohérence avec architecture store-and-forward.

------------------------------------------------------------------------

## 11. Réserves / extensions futures

-   ajout de paramètres avancés acquisition ;
-   ajout de profils de configuration ;
-   extension supervision multi-niveaux ;
-   extension CRC (multi-zones).

Les zones réservées permettent ces évolutions sans rupture.

------------------------------------------------------------------------

# Compléments métier (ajoutés)

> **Statut des compléments métier**
>
> Les sections « Compléments métier » sont informatives et non normatives.
> En cas de divergence, les définitions, types, codes, domaines et règles
> figurant dans les sections normatives du présent document font foi.
> Les « valeurs recommandées » mentionnées dans les compléments ne constituent
> pas des codes protocole tant qu’elles ne sont pas intégrées explicitement
> au mapping normatif.

## Fréquence d'échantillonnage

Utilisation métier 1. adapter la mesure au type d'équipement surveillé
2. arbitrer entre finesse fréquentielle, autonomie et volume de stockage
3. garantir la comparabilité entre campagnes

Exemples métier 1. machine lente ou surveillance simple → fréquence
modérée suffisante 2. machine rapide ou recherche de défauts haute
fréquence → fréquence plus élevée nécessaire

Contrainte Une fréquence trop élevée peut réduire fortement l'autonomie
utile et saturer le stockage sans gain réel si le cas d'usage ne le
justifie pas.

## Axes activés

Utilisation métier 1. sélectionner uniquement les axes utiles à
l'analyse 2. réduire le volume de données lorsque tous les axes ne sont
pas nécessaires

Exemples métier 1. pompe ou ligne surveillée selon une direction
principale → un ou deux axes peuvent suffire 2. machine complexe ou
diagnostic structurel → tri-axes recommandé

Contrainte Le choix des axes doit rester cohérent avec l'orientation
physique du capteur sur l'équipement.

## Plage de mesure MEMS

Utilisation métier 1. éviter la saturation sur les machines très
vibrantes 2. conserver une bonne sensibilité sur les équipements peu
vibrants

Exemples métier 1. faible niveau vibratoire attendu → petite plage
préférable 2. environnement sévère ou chocs possibles → plage plus large
préférable

Contrainte Une plage trop large dégrade la sensibilité utile. Une plage
trop faible augmente le risque de saturation.

## Mode d'acquisition

Utilisation métier 1. adapter la stratégie de mesure à la mission 2.
optimiser l'usage du stockage 3. distinguer surveillance continue et
capture d'événements

Exemples métier 1. campagne d'expertise ponctuelle → mode continu 2.
surveillance longue durée avec peu d'espace → mode événementiel 3.
besoin mixte → continu + événements

Contrainte Le mode retenu doit être cohérent avec la capacité de
stockage disponible et la durée visée de campagne.

## Taille de fenêtre d'analyse

Utilisation métier 1. régler le compromis entre stabilité des
indicateurs et réactivité 2. harmoniser les calculs entre capteurs

Exemple Une fenêtre courte réagit vite mais peut être plus bruitée. Une
fenêtre plus longue stabilise les indicateurs mais masque certains
transitoires.

## Pas de mise à jour des indicateurs live

Utilisation métier 1. adapter la réactivité de la supervision temps réel
2. limiter la charge bus et la charge calcul du capteur

Exemples métier 1. supervision lente de tendance → période longue 2.
phase de test ou mise au point → période plus courte

Contrainte Une mise à jour trop rapide peut surcharger inutilement le
bus sans améliorer l'exploitation réelle.

## Seuils d'alerte et seuil critique

Utilisation métier 1. adapter le comportement d'alerte au type
d'équipement 2. distinguer dérive lente, anomalie sérieuse et état
critique 3. uniformiser les critères entre capteurs comparables

Exemples métier 1. seuil 1 → surveillance renforcée 2. seuil 2 → alarme
maintenance 3. seuil critique → condition potentiellement dangereuse

Contrainte Les seuils doivent être cohérents avec : 1. le type de
grandeur utilisée 2. le point de pose du capteur 3. le type de machine
surveillée

Remarque Un seuil pertinent sur un palier peut être inadapté sur un
châssis ou une tuyauterie.

## Détection saturation activée

Utilisation métier 1. signaler qu'une mesure sort du domaine exploitable
2. éviter les diagnostics faux sur données saturées

Exemple Un choc important ou une plage MEMS trop faible peut provoquer
une saturation.

## Politique de stockage

Utilisation métier 1. gérer le compromis entre richesse des données et
durée de campagne 2. adapter le capteur à une campagne longue ou à une
mission ciblée

Exemples métier 1. brut continu → meilleure richesse analytique,
stockage plus rapide saturé 2. événementiel → autonomie prolongée 3.
mixte → compromis

Contrainte La politique choisie doit rester compatible avec la capacité
réelle du support et la durée visée.

## Taille max campagne

Utilisation métier 1. limiter la taille d'une campagne pour faciliter
l'exploitation 2. éviter une occupation incontrôlée du stockage 3.
découper proprement les acquisitions longues

Exemple Une campagne très longue peut compliquer l'analyse, l'archivage
et la récupération.

## Nom ou ID campagne

Utilisation métier 1. rattacher clairement un capteur à une mission ou à
un essai 2. faciliter l'exploitation multi-capteurs 3. simplifier le
rapprochement avec les données centrales

Exemples métier 1. essai_avant_depart 2. transit_retour 3. expertise_GE2

Contrainte Le nom ou l'identifiant doit rester stable pendant la
campagne acquise.

## Pré-déclenchement

Utilisation métier 1. capturer le contexte juste avant un événement 2.
améliorer l'analyse d'un défaut transitoire

Exemple Un choc, une cavitation ou une anomalie de régime peut être
mieux compris si l'on dispose de quelques instants avant le
déclenchement.

## Post-déclenchement

Utilisation métier 1. conserver l'évolution juste après un événement 2.
éviter de ne stocker qu'un instant isolé peu exploitable

Exemple Après un dépassement de seuil, le post-déclenchement permet
d'observer le retour au régime normal ou la persistance du défaut.

## Activation supervision temps réel

Utilisation métier 1. autoriser ou non la supervision légère sur le bus
2. réserver certains capteurs à une mission purement autonome

Exemple Un capteur inaccessible en exploitation normale peut fonctionner
sans supervision live active.

## Adresse logique capteur

Utilisation métier 1. intégrer proprement le capteur dans l'architecture
RS-485 du navire 2. éviter les conflits d'adressage lors du déploiement
multi-capteurs

Contrainte Toute modification doit être encadrée, documentée et
cohérente avec le plan d'adressage du réseau embarqué.

## Verrouillage configuration

Utilisation métier 1. distinguer clairement une configuration en
préparation d'une configuration réellement engagée 2. éviter les
ambiguïtés au moment de lancer une campagne

Exemple Une campagne ne doit pas démarrer sur une configuration
partiellement préparée ou non validée.

## 4.21 Contexte de campagne

Utilisation métier 1. associer la mesure à une situation d'exploitation
réelle 2. faciliter l'analyse post-campagne 3. distinguer des campagnes
techniquement similaires mais réalisées dans des contextes différents

Exemples de mission_label 1. transit_portuaire 2. essai_GE2 3.
surveillance_pompe_EM 4. manoeuvre_arrivee

Exemples d'operating_mode_code 1. arrêt 2. ralenti 3. régime nominal 4.
montée en charge 5. manoeuvre 6. essai spécifique

Exemples de navigation_zone_code 1. quai 2. port 3. rade 4. côtier 5.
large 6. zone peu profonde 7. chenal

Lien avec l'analyse Ce contexte permet de rapprocher plus facilement les
signatures vibratoires : 1. du régime machine 2. de la situation navire
3. des conditions d'exploitation

Contrainte Ces champs décrivent un contexte déclaratif ou préparé. Ils
ne remplacent pas les données de navigation réelles gérées par la
centrale.

Remarque importante Deux campagnes sur un même équipement peuvent avoir
des signatures très différentes selon : 1. le mode d'exploitation 2. la
zone de navigation 3. la charge ou le régime

## Distinction config préparée / config active

Utilisation métier 1. garantir que la campagne correspond bien à une
configuration validée 2. éviter toute ambiguïté entre intention
opérateur et état réel du capteur

Exemple Une configuration modifiée mais non appliquée ne doit jamais
être considérée comme la référence de la campagne.

## Recommandations de robustesse

Impact métier Une mauvaise configuration peut rendre une campagne
partiellement ou totalement inutilisable, même si le capteur fonctionne
correctement.

Exemples 1. fréquence trop faible → défaut non observable 2. seuils
incohérents → alarmes inutiles ou absentes 3. mode stockage inadapté →
perte de contexte utile
