# TR2 --- Bloc 0 : Identification

## Mapping Modbus RTU --- V1

------------------------------------------------------------------------

## 1. Objet du bloc

Le Bloc 0 fournit les informations d'identification du capteur.

Il permet à la centrale de :

-   identifier de manière unique un équipement ;
-   connaître sa version firmware ;
-   vérifier la compatibilité protocolaire ;
-   récupérer des informations statiques d'inventaire.

Ce bloc est **strictement en lecture seule (RO)**.

------------------------------------------------------------------------

## 2. Base d'adressage

-   **Bloc 0 base = registre 0**

------------------------------------------------------------------------

## 3. Accès

* Bloc entièrement **RO**
* Toute tentative d’écriture sur un registre du Bloc 0 doit être rejetée par une exception Modbus explicite.
* Aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée.
* Une tentative d’écriture ne doit modifier aucun registre ni état interne du bloc.

------------------------------------------------------------------------

## 4. Conventions générales

* Types autorisés : `uint16`, `uint32`, `bitfield16`, `ASCII fixe`
-   `uint32` = **MSW puis LSW**
-   Chaînes ASCII fixes :

  \* codées sur N registres (2 caractères par registre)

  \* padding avec `0x00`

-   Registres réservés :

  \* lecture = `0`

  \* écriture interdite

-   Les champs multi-registres doivent être **cohérents lors d'une même
    lecture**

------------------------------------------------------------------------

### 4.1 Longueurs ASCII du bloc

-   `serial_number` : 16 caractères ASCII fixes
-   `manufacturer` : 8 caractères ASCII fixes

Règles : - 2 caractères par registre - padding `0x00`

------------------------------------------------------------------------

## 5. Mapping complet

  ---------------------------------------------------------------------------------------------
  Offset   Adresse     Nom                      Type         Accès   Description
           absolue                                                   
  -------- ----------- ------------------------ ------------ ------- --------------------------
  0        0           device_id_msw            uint16       RO      Identifiant unique capteur
                                                                     (MSW)

  1        1           device_id_lsw            uint16       RO      Identifiant unique capteur
                                                                     (LSW)

  2        2           hardware_version         uint16       RO      Version hardware

  3        3           firmware_version_major   uint16       RO      Version firmware majeure

  4        4           firmware_version_minor   uint16       RO      Version firmware mineure

  5        5           firmware_version_patch   uint16       RO      Version firmware patch

  6        6           protocol_version         uint16       RO      Version du protocole
                                                                     Modbus TR2

  7        7           device_capabilities      bitfield16   RO      Capacités du capteur

  8        8           serial_number_r0         uint16       RO      Numéro de série
                                                                     (caractères 1-2)

  9        9           serial_number_r1         uint16       RO      Numéro de série
                                                                     (caractères 3-4)

  10       10          serial_number_r2         uint16       RO      Numéro de série
                                                                     (caractères 5-6)

  11       11          serial_number_r3         uint16       RO      Numéro de série
                                                                     (caractères 7-8)

  12       12          serial_number_r4         uint16       RO      Numéro de série
                                                                     (caractères 9-10)

  13       13          serial_number_r5         uint16       RO      Numéro de série
                                                                     (caractères 11-12)

  14       14          serial_number_r6         uint16       RO      Numéro de série
                                                                     (caractères 13-14)

  15       15          serial_number_r7         uint16       RO      Numéro de série
                                                                     (caractères 15-16)

  16       16          manufacturer_r0          uint16       RO      Fabricant (caractères 1-2)

  17       17          manufacturer_r1          uint16       RO      Fabricant (caractères 3-4)

  18       18          manufacturer_r2          uint16       RO      Fabricant (caractères 5-6)

  19       19          manufacturer_r3          uint16       RO      Fabricant (caractères 7-8)

  20       20          reserved_0               uint16       RO      Réservé (0)
  ---------------------------------------------------------------------------------------------

------------------------------------------------------------------------

## 6. Définitions détaillées

### 6.1 `device_capabilities`

  Bit    Signification
  ------ --------------------------------
  0      Support acquisition vibratoire
  1      Support stockage SD
  2      Support supervision
  3      Support diagnostic
  4-15   Réservés

------------------------------------------------------------------------

## 7. Règles d'implémentation

-   Les informations doivent être **statiques pendant le fonctionnement
    normal**
-   Le `device_id` doit être **unique et persistent**
-   Les chaînes ASCII doivent être **lisibles directement sans
    conversion complexe**
-   Aucun champ ne doit dépendre d'un état dynamique du capteur

------------------------------------------------------------------------

## 8. Justification

Le Bloc 0 est volontairement simple et stable afin de :

-   faciliter la découverte du capteur ;
-   permettre une compatibilité inter-versions ;
-   éviter toute ambiguïté d'identification.

------------------------------------------------------------------------

## 9. Réserves / extensions futures

-   Ajout possible de :

  \* identifiant lot de fabrication

  \* date de fabrication

  \* révision PCB

-   Les registres réservés permettent ces évolutions sans rupture

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

## 0.1 Signature équipement / type produit

Interprétation métier Ce champ permet à la centrale de distinguer la
nature de l'équipement interrogé afin d'adapter le traitement des
données et les interfaces opérateur.

Exemples d'utilisation 1. différencier un capteur embarqué sur une
machine d'une centrale fixe 2. adapter automatiquement les vues de
supervision

Valeurs recommandées 1. 1 = capteur vibratoire autonome embarqué 2. 2 =
centrale d'acquisition 3. 3 = passerelle / équipement intermédiaire

## 0.3 Version du protocole

Utilisation côté centrale 1. adaptation du parsing des blocs en fonction
de la version 2. gestion de compatibilité ascendante / descendante

Contrainte Une centrale doit refuser ou dégrader proprement la
communication si la version est non supportée.

## 0.6 Numéro de série unique

Utilisation métier 1. identification physique du capteur lors des
opérations de maintenance 2. traçabilité des données enregistrées 3.
corrélation avec les dossiers de maintenance ou de calibration

Exemple Capteur démonté d'un groupe électrogène → le numéro de série
permet de retrouver son historique vibratoire complet.

Contrainte Le numéro de série ne doit jamais être modifié après
fabrication.

## 0.7 Adresse logique Modbus courante

Utilisation métier 1. identification rapide d'un capteur sur le bus
terrain 2. diagnostic en cas de conflit d'adressage

Contrainte L'adresse doit être cohérente avec la configuration réseau
RS-485 du navire.

## 0.8 Capacité fonctionnelle / profil équipement

Utilisation métier 1. détection automatique des fonctionnalités
disponibles sur le capteur 2. adaptation des algorithmes d'analyse côté
centrale 3. gestion des différences entre générations de capteurs

Exemple Un capteur sans mode événementiel ne doit pas être interrogé
pour ce type de données.

Contrainte Les bits non supportés doivent être ignorés par la centrale.

## 0.10 Référence capteur MEMS

Utilisation métier 1. identification des caractéristiques dynamiques du
capteur (bande passante, bruit, plage) 2. adaptation des algorithmes FFT
et filtrage 3. diagnostic en cas de dérive de mesure

Exemple Deux capteurs avec des MEMS différents peuvent produire des
signatures vibratoires différentes.

## 0.11 Nombre d'axes supportés

Utilisation métier 1. adaptation des traitements vibratoires (mono-axe
vs tri-axes) 2. sélection des axes pertinents pour l'analyse

Exemple 1. pompe → souvent mono-axe suffisant 2. moteur ou structure →
tri-axes recommandé

## 0.12 Taille de stockage disponible

Utilisation métier 1. estimation de la durée maximale de campagne
possible 2. gestion des stratégies d'acquisition (continue vs
événementielle)

Exemple Un stockage faible impose un mode événementiel ou une fréquence
d'échantillonnage réduite.

## 0.13 Identifiant de lot / fabrication

Utilisation métier 1. suivi des séries de fabrication 2. détection de
défauts série éventuels 3. gestion des rappels ou corrections

Exemple Un lot défectueux de MEMS peut être identifié via ce champ.

## 0.14 Localisation d'installation du capteur (statique)

Interprétation métier Cette section décrit l'emplacement physique du
capteur sur le navire, indépendamment des campagnes de mesure.

Utilisation métier 1. association des données vibratoires à un
équipement réel 2. analyse de tendance par machine 3. diagnostic ciblé
(pompe, moteur, ligne d'arbre, etc.)

Exemples d'équipements (asset_label) 1. "GE2" → groupe électrogène 2 2.
"PPE_EM" → pompe eau de mer 3. "LIGNE_ARB" → ligne d'arbre propulsion 4.
"COMP_AIR" → compresseur air

Exemples de points de pose (installation_location_label) 1.
"palier_avant" 2. "palier_arriere" 3. "carter" 4. "chassis" 5.
"support_moteur"

Interprétation mounting_zone_code 1. palier → zone fortement
représentative des défauts mécaniques 2. carter → mesure globale de
vibration 3. châssis → transmission structurelle 4. tuyauterie →
vibrations induites (cavitation, flux)

Contraintes métier 1. ces informations doivent rester stables dans le
temps 2. toute modification correspond à un déplacement physique du
capteur 3. elles ne doivent pas dépendre du contexte de campagne

Lien avec l'analyse La qualité de ces informations conditionne
directement la pertinence des diagnostics vibratoires.
