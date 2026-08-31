# MSM – Capteur de vibration TR2

## Charte de typage Modbus RTU



Version de référence pour la spécification et l’implémentation des registres Modbus entre la centrale et les capteurs TR2.



---



## 1. Objet



Cette charte fixe les règles de typage applicables à l’ensemble du protocole Modbus RTU du projet.



Objectifs :



- éliminer toute ambiguïté d’interprétation,

- garantir la cohérence firmware ↔ centrale,

- stabiliser les mappings dans le temps,

- permettre l’automatisation complète des tests,

- assurer la robustesse en environnement terrain.



---



## 2. Types autorisés



Types strictement autorisés :



- `uint16`

- `int16`

- `uint32`

- `bitfield16`

- `enum16`

- `ASCII fixe`



Tout autre type est interdit sans évolution formelle de la présente charte.

### 2.1 Notation de regroupement dans les mappings

La notation `uint16[n]` est autorisée uniquement comme **notation documentaire de regroupement**.

Elle signifie : `n` registres consécutifs, chacun de type `uint16`.

Elle ne constitue pas un type protocolaire supplémentaire et ne modifie ni l’ordre des registres ni les règles d’accès applicables à chacun d’eux.




---



## 3. Principe fondamental



Un registre Modbus transporte **exactement 16 bits**.



Son interprétation dépend **exclusivement du type déclaré dans le mapping**.



Règles impératives :



- chaque champ doit déclarer explicitement son type,

- aucune interprétation implicite n’est autorisée,

- aucune conversion cachée ne doit être faite côté centrale ou firmware.



---



## 4. Cohérence de lecture (règle globale critique)



Lors d’une lecture Modbus multi-registres :



👉 tous les registres retournés doivent correspondre à un **même instant logique**.



Le firmware doit garantir :



- cohérence intra-champ (`uint32`),

- cohérence inter-champs d’un même bloc,

- absence de mélange ancien/nouveau dans une même réponse.



### Implémentation recommandée



- snapshot mémoire du bloc,

- ou buffer miroir exposé,

- ou verrou logique de lecture.



### Recommandation de testabilité pour les blocs dynamiques



Pour les blocs dynamiques ou susceptibles d’évoluer pendant la lecture, il est recommandé de garantir la cohérence de réponse à l’échelle du bloc complet via :



- snapshot mémoire,

- buffer miroir,

- ou verrou logique de lecture.



Cette recommandation concerne en priorité :

- Bloc 1 — État système

- Bloc 2 — Temps

- Bloc 3 — Supervision vibratoire

- Bloc 6 — Inventaire campagnes

- Bloc 7 — Diagnostic



---



## 5. Règles par type



### 5.1 `uint16`



- plage : `0 .. 65535`

- usage :

&nbsp; - compteurs

&nbsp; - codes

&nbsp; - tensions (mV)

&nbsp; - tailles

&nbsp; - durées positives



---



### 5.2 `int16`



- plage : `-32768 .. +32767`

- codage : complément à deux



Usage :



- grandeurs physiques signées



Cas normalisé :



- température → `0,1 °C`



⚠️ Règle forte :



- interdiction d’utiliser un `uint16` biaisé



---



### 5.3 `uint32`



- plage : `0 .. 4 294 967 295`

- encodage :

registre N = MSW

registre N+1 = LSW





Usage :



- timestamps

- tailles

- durées longues

- identifiants étendus



---



### 5.4 `bitfield16`



- chaque bit documenté

- bits réservés = `0`

- pas de machine d’état implicite



---



### 5.5 `enum16`



- valeur `0` définie obligatoirement

- stabilité dans le temps

- plages non utilisées réservées



---



### 5.6 `ASCII fixe`



- 2 caractères / registre

- dans chaque registre, le premier caractère est placé dans l’octet de poids fort (bits 15..8)

- le second caractère est placé dans l’octet de poids faible (bits 7..0)

- exemple : `"AB"` est encodé `0x4142`

- si un seul caractère reste à encoder, l’octet faible est paddé à `0x00` (exemple : `"A"` → `0x4100`)

- padding `0x00`

- longueur fixe obligatoire

- ASCII uniquement



---



## 6. Convention des grandeurs physiques



### 6.1 Interdiction des floats



Toutes les grandeurs sont codées en entier avec :



- unité explicite

- facteur d’échelle explicite



Exemples :



- température → `0,1 °C`

- tension → `mV`

- accélération → `mg`

- temps → `s`



---



### 6.2 Cohérence inter-blocs (règle critique)



Une même grandeur physique doit être exprimée :



👉 avec **la même unité dans tous les blocs**



Exemples :



- les seuils du Bloc 4 doivent utiliser **la même unité que les mesures du Bloc 3**

- les timestamps doivent tous référencer **le Bloc 2**



Toute divergence est interdite.



---



## 6.3 Référence temporelle commune



La référence temporelle commune du protocole est :



- **Epoch TR2 = 2020-01-01 00:00:00 UTC**



Règles :

- tous les timestamps exprimés en `uint32` et référencés au temps du protocole utilisent cette origine ;

- l’unité est la **seconde** sauf mention explicite contraire ;

- aucun bloc ne doit utiliser une autre époque implicite.



---



## 7. Valeurs non renseignées



### 7.1 Principe



`0` peut signifier “non renseigné” uniquement si explicitement défini.



---



### 7.2 Champs physiques signés



Pour les champs `int16` :



👉 `0` est une valeur physique valide



Donc :



- pas de surcharge implicite

- si besoin :

&nbsp; - flag de validité séparé

&nbsp; - ou convention documentée



---



## 8. Atomicité des `uint32` (point critique terrain)



Règle normative :



- MSW et LSW doivent provenir du **même instant logique**



Interdiction :



- MSW ancien + LSW nouveau



---



### Implémentation recommandée



- copie atomique

- double buffer

- verrou logiciel



---



## 9. Règles de cohérence documentaire



Chaque champ doit documenter :



- type

- unité

- facteur d’échelle

- ordre des mots

- règle de validité



Tous les artefacts doivent être cohérents :



- Markdown

- CSV

- firmware

- centrale

- tests



---



## 10. Règles firmware



- aucune conversion implicite

- respect strict du mapping

- cohérence des réponses Modbus



---



## 11. Règles centrale



- décodage strict selon type

- aucune heuristique

- respect MSW/LSW



---



## 12. Règles de test (renforcées)



Les tests doivent couvrir :



- bornes min/max

- valeurs négatives (`int16`)

- cohérence `uint32`

- cohérence multi-registres

- cohérence inter-blocs

- unités identiques entre blocs



---



## 13. Décisions projet



- `int16` validé officiellement

- température → `int16`

- suppression des anciens biais

- unités vibratoires unifiées



---


## 14. Gestion des accès invalides

Toute requête Modbus visant :
- une adresse inexistante ;
- un registre en lecture seule avec une opération d’écriture ;
- un registre réservé avec une opération d’écriture ;
- ou plus généralement un accès non autorisé par le mapping,

doit être rejetée par une exception Modbus standard.

Aucune écriture silencieusement ignorée, partiellement exécutée
ou implicitement corrigée n’est autorisée.

Une requête rejetée ne doit modifier aucun registre ni état interne.

Une valeur métier hors domaine écrite dans un registre explicitement RW
ne constitue pas, à elle seule, un accès Modbus invalide.

Son traitement est défini par les règles fonctionnelles du bloc concerné.


## 15. Conclusion



Cette charte est la référence contractuelle du protocole TR2.



Aucune dérogation n’est autorisée sans validation formelle.



