# TR2 — Bloc 5 : Commandes

## Mapping Modbus RTU — V1



---



## 1\. Objet du bloc



Le Bloc 5 regroupe les **commandes explicites** envoyées par la centrale au capteur.



Ce bloc ne porte **aucun paramètre métier permanent**. Il sert uniquement à :



* déclencher une action ;
* suivre son exécution ;
* exposer son résultat ;
* mémoriser un historique minimal de la dernière commande terminée.



Le bloc est conçu selon une logique **transactionnelle**, robuste vis-à-vis des répétitions Modbus, des pertes de réponse et des ambiguïtés d’exécution.



---



## 2\. Base d’adressage



* **Bloc 5 base = registre 5000**



---



## 3\. Accès



* Zone de requête : **RW**
* Zone d’état courant : **RO**
* Zone d’historique minimal : **RO**



---



## 4\. Conventions générales



* une seule commande active à la fois ;
* déclenchement **explicite** uniquement ;
* `transaction\_id` obligatoire ;
* séparation entre :

  - zone de requête ;

  - zone d’état courant ;

  - zone d’historique minimal ;

* clé de confirmation pour commandes protégées ;
* `uint32` exposés en **MSW puis LSW** ;
* les champs `uint32` de ce bloc doivent être exposés de manière cohérente lors d’une même lecture Modbus, conformément à la règle générale d’atomicité logique du protocole ;
* les registres réservés doivent toujours être lus à `0` ;
* toute tentative d’écriture sur un registre réservé doit être rejetée par une exception Modbus explicite, y compris si la valeur écrite est `0` ;
* aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée ;
* une tentative d’écriture sur un registre réservé ne doit modifier aucun état interne ni registre du bloc.



### 4.1 Philosophie d’usage



Les écritures dans les autres blocs **ne déclenchent aucune action par elles-mêmes**.



Exemples :



* l’écriture de l’heure préparée dans le Bloc 2 ne synchronise pas l’horloge ;
* l’écriture de la configuration préparée dans le Bloc 4 ne l’applique pas ;
* l’inventaire du Bloc 6 n’est jamais modifié implicitement par simple lecture/écriture hors commande dédiée.



Le déclenchement effectif passe par le **Bloc 5**.



### 4.2 Idempotence



Une commande avec un `transaction\_id` déjà traité ne doit jamais être exécutée une seconde fois.



Le capteur doit répondre avec :

* `cmd\_status = terminé`
* en réutilisant le résultat précédent.



---



## 5\. Taille du bloc



* **20 registres 16 bits**
* offsets `0` à `19`
* adresses absolues `5000` à `5019`



---



## 6\. Mapping complet



| Offset | Adresse absolue | Nom registre | Type | Accès | Description |

|---:|---:|---|---|---|---|

| 0 | 5000 | `cmd\_request\_code` | uint16 | RW | Code commande demandé par la centrale |

| 1 | 5001 | `cmd\_request\_transaction\_id` | uint16 | RW | Identifiant de transaction de la commande |

| 2 | 5002 | `cmd\_request\_param1` | uint16 | RW | Paramètre 1 |

| 3 | 5003 | `cmd\_request\_param2` | uint16 | RW | Paramètre 2 |

| 4 | 5004 | `cmd\_request\_param3\_msw` | uint16 | RW | Paramètre 3, mot fort |

| 5 | 5005 | `cmd\_request\_param3\_lsw` | uint16 | RW | Paramètre 3, mot faible |

| 6 | 5006 | `cmd\_request\_confirm\_key` | uint16 | RW | Clé de confirmation pour commande protégée |

| 7 | 5007 | `cmd\_request\_control` | bitfield16 | RW | Bits de soumission / annulation / nettoyage |

| 8 | 5008 | `cmd\_active\_code` | uint16 | RO | Code de la commande active ou dernière commande prise en compte |

| 9 | 5009 | `cmd\_active\_transaction\_id` | uint16 | RO | Transaction ID de la commande active / dernière prise en compte |

| 10 | 5010 | `cmd\_status` | enum16 | RO | Statut de traitement |

| 11 | 5011 | `cmd\_result\_code` | enum16 | RO | Code résultat détaillé |

| 12 | 5012 | `cmd\_result\_detail` | uint16 | RO | Détail complémentaire spécifique |

| 13 | 5013 | `cmd\_engine\_flags` | bitfield16 | RO | Flags moteur de commande |

| 14 | 5014 | `cmd\_last\_code` | uint16 | RO | Dernière commande terminée |

| 15 | 5015 | `cmd\_last\_transaction\_id` | uint16 | RO | Transaction ID de la dernière commande terminée |

| 16 | 5016 | `cmd\_last\_status\_final` | enum16 | RO | Statut final de la dernière commande |

| 17 | 5017 | `cmd\_last\_result\_code` | enum16 | RO | Résultat final de la dernière commande |

| 18 | 5018 | `cmd\_last\_timestamp\_msw` | uint16 | RO | Timestamp de fin de dernière commande, mot fort |

| 19 | 5019 | `cmd\_last\_timestamp\_lsw` | uint16 | RO | Timestamp de fin de dernière commande, mot faible |



---



## 7\. Définitions détaillées des registres



### 7.1 `cmd\_request\_code`



* **Type** : `uint16`
* **Offset** : `0`
* **Adresse absolue** : `5000`
* **Accès** : `RW`



| Valeur | Signification |

|---:|---|

| 0 | aucune commande |

| 1 | appliquer configuration préparée |

| 2 | synchroniser heure |

| 3 | démarrer acquisition |

| 4 | arrêter acquisition |

| 5 | lancer autotest |

| 6 | acquitter défaut / alarme acquittable |

| 7 | rafraîchir indicateurs / états calculés |

| 8 | entrer en maintenance |

| 9 | sortir maintenance |

| 10 | reset logiciel contrôlé |

| 11 | RAZ statistiques |

| 12..65535 | réservés |



---



### 7.2 `cmd\_request\_transaction\_id`



* **Type** : `uint16`
* **Offset** : `1`
* **Adresse absolue** : `5001`
* **Accès** : `RW`



La centrale doit **modifier cette valeur à chaque nouvelle commande**.



Ce champ est obligatoire pour :

* distinguer deux commandes successives identiques ;
* éviter l’ambiguïté en cas de répétition de trame ;
* permettre la corrélation stricte entre requête et réponse.



---



### 7.3 `cmd\_request\_param1`, `cmd\_request\_param2`



* **Type** : `uint16`
* **Offsets** : `2`, `3`
* **Adresses absolues** : `5002`, `5003`
* **Accès** : `RW`



Paramètres courts optionnels dépendant de la commande.



Convention :

* si non utilisés : `0`.



---



### 7.4 `cmd\_request\_param3\_msw`, `cmd\_request\_param3\_lsw`



* **Type** : `uint32`
* **Offsets** : `4..5`
* **Adresses absolues** : `5004..5005`
* **Accès** : `RW`



Paramètre long optionnel pour extension future ou usage spécifique commande.



Convention :

* si non utilisé : `0`.



Codage :

* `5004` = MSW
* `5005` = LSW



---



### 7.5 `cmd\_request\_confirm\_key`



* **Type** : `uint16`
* **Offset** : `6`
* **Adresse absolue** : `5006`
* **Accès** : `RW`



Champ utilisé uniquement pour les commandes protégées.



| Valeur | Signification |

|---:|---|

| `0x0000` | aucune confirmation |

| `0xA55A` | confirmation valide |



Commandes protégées en version retenue :

* `10` : reset logiciel contrôlé ;
* `11` : RAZ statistiques.



---



### 7.6 `cmd\_request\_control`



* **Type** : `bitfield16`
* **Offset** : `7`
* **Adresse absolue** : `5007`
* **Accès** : `RW`



| Bit | Nom | Signification |

|---:|---|---|

| 0 | `submit` | Demande de prise en compte de la commande |

| 1 | `cancel\_request` | Demande d’annulation si la commande courante est annulable |

| 2 | `clear\_request\_fields` | Demande de remise à zéro des champs de requête |

| 3 | `reserved` | réservé |

| 4..15 | `reserved` | réservés, doivent être écrits à 0 |



La commande n’est prise en compte que si :

* `submit = 1` ;
* `cmd\_request\_code != 0` ;
* `transaction\_id` valide ;
* contexte compatible.



### Règle de prise en compte du bit `submit`



La prise en compte d’une commande se fait sur **front montant** du bit `submit`.



Règles associées :



* une commande n’est évaluée que lors du passage de `submit` de `0` à `1` ;
* le maintien de `submit = 1` ne doit pas provoquer de réexécution répétée ;
* après prise en compte, le firmware peut :

  - soit remettre automatiquement `submit` à `0`,

  - soit ignorer tout nouvel examen tant qu’un nouveau front montant n’a pas été observé.



#### Conséquence normative



Une commande identique avec le même `transaction\_id` ne doit jamais être exécutée une seconde fois, même si `submit` reste à `1`.



Le firmware remet automatiquement `submit` à `0` après prise en compte.



---



### 7.7 `cmd\_active\_code`



* **Type** : `uint16`
* **Offset** : `8`
* **Adresse absolue** : `5008`
* **Accès** : `RO`



Code de la commande actuellement traitée, ou de la dernière commande prise en compte tant qu’aucune nouvelle commande n’a été acceptée.



---



### 7.8 `cmd\_active\_transaction\_id`



* **Type** : `uint16`
* **Offset** : `9`
* **Adresse absolue** : `5009`
* **Accès** : `RO`



Identifiant de transaction de la commande active / dernière commande prise en compte.



---



### 7.9 `cmd\_status`



* **Type** : `enum16`
* **Offset** : `10`
* **Adresse absolue** : `5010`
* **Accès** : `RO`



| Valeur | Signification |

|---:|---|

| 0 | aucune commande |

| 1 | commande reçue |

| 2 | commande acceptée |

| 3 | commande en cours |

| 4 | commande terminée avec succès |

| 5 | commande refusée |

| 6 | commande échouée |

| 7 | commande inconnue |

| 8 | commande non autorisée dans l’état courant |



---



### 7.10 `cmd\_result\_code`



* **Type** : `enum16`
* **Offset** : `11`
* **Adresse absolue** : `5011`
* **Accès** : `RO`



| Valeur | Signification |

|---:|---|

| 0 | succès |

| 1 | commande inconnue |

| 2 | paramètre invalide |

| 3 | état incompatible |

| 4 | configuration invalide |

| 5 | acquisition en cours |

| 6 | SD absente |

| 7 | mémoire insuffisante |

| 8 | défaut critique actif |

| 9 | commande protégée / confirmation absente |

| 10 | timeout interne |

| 11 | échec autotest |

| 12 | horloge non disponible |

| 13 | commande déjà en cours |

| 14 | transaction\_id invalide |

| 15 | commande non annulable |

| 16 | défaut non acquittable |

| 17 | mode maintenance requis |

| 18 | mode maintenance actif : commande interdite |

| 19 | synchronisation préparée absente |

| 20 | configuration préparée incomplète |

| 21 | arrêt impossible : acquisition non active |

| 22 | démarrage impossible : aucune configuration active valide |

| 23..65535 | réservés |



---



### 7.11 `cmd\_result\_detail`



* **Type** : `uint16`
* **Offset** : `12`
* **Adresse absolue** : `5012`
* **Accès** : `RO`



Détail complémentaire dépendant de la commande.



Exemples :

* code du défaut concerné pour acquittement ;
* identifiant du sous-test en échec pour autotest ;
* index du paramètre fautif ;
* `0` si non applicable.



---



### 7.12 `cmd\_engine\_flags`



* **Type** : `bitfield16`
* **Offset** : `13`
* **Adresse absolue** : `5013`
* **Accès** : `RO`



| Bit | Signification |

|---:|---|

| 0 | moteur de commande prêt |

| 1 | commande en cours |

| 2 | la commande courante requiert une confirmation |

| 3 | mode maintenance actif |

| 4 | acquisition active |

| 5 | défaut critique actif |

| 6 | configuration active valide |

| 7 | synchronisation préparée disponible |

| 8 | configuration préparée disponible |

| 9 | annulation supportée pour la commande courante |

| 10 | journalisation commande effectuée |

| 11..15 | réservés |



---



### 7.13 `cmd\_last\_code`



* **Type** : `uint16`
* **Offset** : `14`
* **Adresse absolue** : `5014`
* **Accès** : `RO`



Dernière commande terminée, quel que soit son résultat final.



---



### 7.14 `cmd\_last\_transaction\_id`



* **Type** : `uint16`
* **Offset** : `15`
* **Adresse absolue** : `5015`
* **Accès** : `RO`



Identifiant de transaction de la dernière commande terminée.



---



### 7.15 `cmd\_last\_status\_final`



* **Type** : `enum16`
* **Offset** : `16`
* **Adresse absolue** : `5016`
* **Accès** : `RO`



Même table de valeurs que `cmd\_status`, mais mémorisée comme **état final** de la dernière commande terminée.



---



### 7.16 `cmd\_last\_result\_code`



* **Type** : `enum16`
* **Offset** : `17`
* **Adresse absolue** : `5017`
* **Accès** : `RO`



Code résultat final de la dernière commande terminée.



---



### 7.17 `cmd\_last\_timestamp\_msw`, `cmd\_last\_timestamp\_lsw`



* **Type** : `uint32`
* **Offsets** : `18..19`
* **Adresses absolues** : `5018..5019`
* **Accès** : `RO`
* **Unité** : `s`
* **Référence** : `Epoch TR2`



Timestamp de fin de la dernière commande terminée.



Codage :

* `5018` = MSW
* `5019` = LSW



La base temporelle utilisée est celle définie dans le Bloc 2.



---



## 8\. Définition des commandes



### 8.1 Commande 1 — Appliquer configuration préparée



**Code** : `1`



**Paramètres** :

* `param1 = 0`
* `param2 = 0`
* `param3 = 0`



**Conditions d’acceptation** :

* acquisition arrêtée ;
* configuration préparée complète ;
* contenu du Bloc 4 valide ;
* recalcul interne du CRC de la zone préparée conforme à la valeur exposée dans `prepared\_config\_crc`.



**Effets** :

* validation de la configuration préparée ;
* bascule en configuration active ;
* mise à jour des indicateurs et états associés.



**Refus typiques** :

* `3` état incompatible ;
* `5` acquisition en cours ;
* `20` configuration préparée incomplète ;
* `4` configuration invalide.



**Règle de sûreté** :



Le firmware doit recalculer le CRC de la zone préparée au moment de l’application effective.



Si le CRC recalculé ne correspond pas à `prepared\_config\_crc` :

* la configuration doit être refusée ;
* `cmd\_result\_code` doit indiquer une erreur de configuration invalide ou incohérente ;
* `config\_state` doit passer à `ERREUR\_VALIDATION` ou rester non appliqué selon la politique retenue.



---



### 8.2 Commande 2 — Synchroniser heure



**Code** : `2`



**Paramètres** :

* aucun dans le Bloc 5 ;
* l’heure cible est préparée dans le Bloc 2.



**Conditions d’acceptation** :

* heure préparée présente ;
* horloge disponible.



**Effets** :

* application de l’heure préparée dans le Bloc 2 ;
* mise à jour de l’état de synchronisation ;
* journalisation de l’opération.



**Refus typiques** :

* `12` horloge non disponible ;
* `19` synchronisation préparée absente.



---



### 8.3 Commande 3 — Démarrer acquisition



**Code** : `3`



**Paramètres** :

* `param1 = 0` par défaut ;
* `param2 = 0` ;
* `param3 = 0`.



**Conditions d’acceptation** :

* configuration active valide ;
* support SD exploitable ;
* absence de défaut critique bloquant ;
* acquisition non déjà active.



**Effets** :

* démarrage de l’acquisition ;
* ouverture d’une nouvelle campagne selon la logique firmware ;
* mise à jour de l’état système.



**Refus typiques** :

* `22` démarrage impossible : aucune configuration active valide ;
* `6` SD absente ;
* `7` mémoire insuffisante ;
* `8` défaut critique actif ;
* `3` état incompatible.



---



### 8.4 Commande 4 — Arrêter acquisition



**Code** : `4`



**Paramètres** : aucun.



**Conditions d’acceptation** :

* acquisition active.



**Effets** :

* arrêt propre de l’acquisition ;
* vidage des buffers ;
* fermeture des fichiers ;
* mise en cohérence de la campagne en cours.



**Refus typique** :

* `21` arrêt impossible : acquisition non active.



---



### 8.5 Commande 5 — Lancer autotest



**Code** : `5`



**Paramètres** :

* `param1 = 0` : autotest standard complet ;
* autre valeur : masque de sous-tests si cette extension est implémentée.



**Effets** :

* lancement de la séquence d’autotest ;
* publication du détail principalement dans le Bloc 7 Diagnostic.



**Refus typiques** :

* `3` état incompatible ;
* `10` timeout interne ;
* `11` échec autotest.



---



### 8.6 Commande 6 — Acquitter défaut / alarme



**Code** : `6`



**Paramètres** :

* `param1` : code défaut à acquitter ;
* `param2 = 0` : acquittement unitaire ;
* `param2 = 1` : acquittement global des défauts acquittables.



**Conditions d’acceptation** :

* défaut concerné présent ;
* défaut de type acquittable.



**Refus typiques** :

* `16` défaut non acquittable ;
* `2` paramètre invalide.



**Règle** :

L’acquittement ne supprime jamais la cause si celle-ci reste présente.



---



### 8.7 Commande 7 — Rafraîchir indicateurs



**Code** : `7`



**Paramètres** : aucun.



**Effets** :

* recalcul / rafraîchissement des indicateurs synthétiques ;
* aucune modification de configuration.



---



### 8.8 Commande 8 — Entrer en maintenance



**Code** : `8`



**Paramètres** : aucun.



**Effets** :

* activation du mode maintenance.



**Politique recommandée** :

* accepter uniquement acquisition arrêtée, sinon refus explicite.



---



### 8.9 Commande 9 — Sortir maintenance



**Code** : `9`



**Paramètres** : aucun.



**Effets** :

* désactivation du mode maintenance.



---



### 8.10 Commande 10 — Reset logiciel contrôlé



**Code** : `10`



**Paramètres** :

* `confirm\_key = 0xA55A` obligatoire.



**Conditions d’acceptation** :

* acquisition arrêtée ;
* aucune opération critique non terminée.



**Effets** :

* journalisation ;
* accusé de prise en compte ;
* redémarrage logiciel contrôlé.



**Refus typiques** :

* `9` commande protégée / confirmation absente ;
* `5` acquisition en cours.



---



### 8.11 Commande 11 — RAZ statistiques



**Code** : `11`



**Paramètres** :

* `confirm\_key = 0xA55A` obligatoire ;
* `param1` : masque éventuel futur ; sinon `0`.



**Effets** :

* remise à zéro des statistiques de service non critiques.



**Restriction** :

Cette commande ne doit pas effacer :

* les campagnes ;
* l’identité capteur ;
* la configuration ;
* les journaux critiques.



---



## 9\. Séquence d’utilisation recommandée côté centrale



### 9.1 Séquence nominale



1. écrire `cmd\_request\_code` ;
2. écrire `cmd\_request\_transaction\_id` ;
3. écrire les paramètres éventuels ;
4. écrire `cmd\_request\_confirm\_key` si nécessaire ;
5. positionner `cmd\_request\_control.submit = 1` ;
6. lire `cmd\_active\_transaction\_id`, `cmd\_status`, `cmd\_result\_code` ;
7. attendre un état final :

   - `4` succès ;

   - `5` refus ;

   - `6` échec.



### 9.2 Règle de corrélation



La centrale ne doit considérer la réponse valide que si :

* `cmd\_active\_transaction\_id == transaction\_id envoyé`.



---



## 10\. Règles firmware à figer



### 10.1 Prise en compte d’une commande



Une commande est considérée comme nouvelle si :

* `submit = 1` ;
* `cmd\_request\_code != 0` ;
* `transaction\_id` valide ;
* contexte compatible.



### 10.2 Concurrence



Si une nouvelle commande arrive alors qu’une commande est déjà en cours :

* refus avec `cmd\_result\_code = 13`.



### 10.3 Traçabilité



Les commandes sensibles doivent être journalisées.



### 10.4 Absence d’effets implicites



* arrêt acquisition ≠ purge ;
* RAZ statistiques ≠ suppression campagnes ;
* acquittement ≠ suppression de la cause.



### 10.5 Champs réservés

Les **registres réservés** doivent toujours être lus à `0`.

Toute tentative d’écriture sur un registre réservé doit être rejetée
par une exception Modbus explicite, y compris si la valeur écrite est `0`.

Les **bits réservés situés dans un registre explicitement RW** doivent être écrits à `0`.

La présence d’un bit réservé à `1` dans une valeur écrite sur un registre RW constitue une **valeur sémantiquement invalide**, et non un accès Modbus invalide.

Dans ce cas :
* l’écriture Modbus du registre reste un accès valide ;
* la commande ou l’action associée ne doit pas être exécutée ;
* le refus doit être exposé par la logique fonctionnelle du Bloc 5, avec `cmd_result_code = 2` (`paramètre invalide`) lorsque la valeur est soumise comme commande.



---



## 11\. Justification



Le Bloc 5 est défini comme un **bloc transactionnel compact** offrant :

* robustesse Modbus ;
* simplicité firmware ;
* lisibilité côté centrale ;
* base propre pour l’industrialisation et les essais.



Son périmètre reste volontairement resserré :

* pas de file de commandes ;
* pas de mini-protocole secondaire ;
* pas de paramètres permanents ;
* extensions futures possibles sans rupture de mapping.



---



## 12\. Réserves / extensions futures



* ajout possible de nouvelles commandes ;
* ajout futur de paramètres longs dédiés ;
* extension de l’historique de commande ;
* extension transactionnelle sans rupture du mapping courant.



---

# Compléments métier (ajoutés)

> **Statut des compléments métier**
>
> Les sections « Compléments métier » sont informatives et non normatives.
> En cas de divergence, les définitions, types, codes, domaines et règles
> figurant dans les sections normatives du présent document font foi.
> Les « valeurs recommandées » mentionnées dans les compléments ne constituent
> pas des codes protocole tant qu’elles ne sont pas intégrées explicitement
> au mapping normatif.

## Commande démarrage acquisition

Utilisation métier

1. démarrer une campagne d’acquisition
2. valider le passage en état actif

Contrainte
La commande ne doit être acceptée que si :

1. une configuration valide est active
2. le stockage est disponible
3. le capteur est dans un état compatible

Exemple
Un démarrage avec carte SD absente doit être refusé.

## Commande arrêt acquisition

Utilisation métier

1. arrêter proprement une campagne
2. garantir l’intégrité des données enregistrées

Contrainte
L’arrêt doit assurer :

1. la fermeture correcte des fichiers
2. la cohérence des métadonnées

Exemple
Un arrêt brutal peut rendre une campagne inutilisable.

## Application configuration préparée

Utilisation métier

1. passer d’une configuration préparée à une configuration active
2. sécuriser la préparation de campagne

Contrainte

1. ne doit pas être autorisé pendant une acquisition active
2. doit être atomique

Exemple
Une configuration partiellement appliquée doit être impossible.

## Synchronisation horloge

Utilisation métier

1. resynchroniser le capteur avec la centrale
2. garantir la cohérence temporelle

Exemple
Synchronisation avant démarrage campagne recommandée.

Contrainte
La synchronisation doit être confirmée par le Bloc 2.

## Reset capteur (soft reset)

Utilisation métier

1. récupérer un capteur dans un état incohérent
2. appliquer proprement certaines modifications

Contrainte

1. ne doit pas être déclenché pendant une acquisition
2. doit préserver l’intégrité des données stockées

Exemple
Utilisé après une erreur système.

## Reset logiciel contrôlé

Utilisation métier

1. redémarrer le capteur proprement
2. récupérer un état cohérent après erreur

Contrainte

1. ne doit pas être déclenché pendant une acquisition
2. doit préserver l’intégrité des données stockées

Remarque
Cette commande ne réinitialise pas la configuration.
Toute remise à zéro de configuration nécessitera une commande dédiée dans une future version du protocole.

## Effacement stockage / campagnes

Utilisation métier

1. libérer de l’espace
2. préparer une nouvelle campagne

Contrainte (CRITIQUE)

1. action irréversible
2. doit nécessiter confirmation (double commande ou clé logique)
3. ne doit jamais être déclenchée accidentellement

Exemple
Effacement complet avant nouvelle mission.

Recommandation
Prévoir une protection contre déclenchement involontaire.

## Commande diagnostic

Utilisation métier

1. déclencher un auto-test
2. vérifier l’état du capteur avant campagne

Exemple
Test du capteur MEMS, de la mémoire, de la carte SD.

Contrainte
Le résultat doit être visible dans le Bloc 7 (diagnostic).

## Application configuration préparée

Utilisation métier

1. valider et appliquer une configuration préparée
2. sécuriser le passage en configuration active

Contrainte

1. ne doit pas être autorisé pendant une acquisition active
2. doit être atomique

## État dernière commande

Utilisation métier

1. savoir si une commande a été correctement exécutée
2. diagnostiquer un refus ou une erreur

Valeurs recommandées

1. succès
2. refus
3. erreur

Exemple
Un démarrage refusé doit être explicite.

## Code erreur commande

Utilisation métier

1. comprendre la cause d’un échec
2. faciliter le diagnostic terrain

Exemples

1. stockage indisponible
2. configuration invalide
3. état incompatible

Contrainte
Les codes doivent être stables et documentés.

## Verrouillage commandes

Utilisation métier

1. éviter les actions involontaires
2. sécuriser le système en exploitation

Exemple
Verrouillage activé en mode production.

Contrainte
Certaines commandes critiques doivent être bloquées si verrou actif.

## Sécurité des commandes

Certaines commandes peuvent avoir un impact critique sur les données ou le fonctionnement du capteur.

Catégories de commandes

1. non critiques (lecture, diagnostic)
2. critiques (reset, configuration)
3. destructives (effacement stockage)

Recommandations

1. exiger une validation explicite pour les commandes critiques
2. journaliser l’exécution des commandes
3. empêcher les commandes incompatibles avec l’état courant

Exemple
Un effacement stockage ne doit jamais être possible sans confirmation explicite.

## Cohérence avec l’état système

Utilisation métier

1. garantir que les commandes sont exécutées dans un état valide
2. éviter les transitions incohérentes

Exemple
Impossible de démarrer une acquisition si le capteur est en défaut critique.

Lien
Ce comportement doit être cohérent avec le Bloc 1.

## Séquencement des commandes

Utilisation métier

1. assurer un déroulement correct des opérations
2. éviter les erreurs opérateur

Exemple de séquence

1. préparation configuration
2. validation
3. application
4. synchronisation temps
5. démarrage acquisition

Recommandation
La centrale doit guider ce séquencement.

## Limitation fonctionnelle V1

Le Bloc 5 V1 ne couvre pas certaines commandes potentielles, notamment :

1. effacement du stockage ou des campagnes
2. remise à zéro complète de la configuration
3. validation indépendante d’une configuration sans application

Ces fonctionnalités pourront être introduites dans une version ultérieure du protocole si nécessaire.

En V1, toute action doit passer exclusivement par les commandes définies dans `cmd\_request\_code`.

