# Projet MSM — Capteur de vibration TR2

## Gel S6 — IHM Web locale / expérience opérateur

Date de gel : 2026-09-09

Ce document clôture **S6 — IHM Web locale / expérience opérateur**. Il complète les gels S0 à S5. Il ne modifie ni la spécification Modbus RTU V1, ni les gels firmware, ni les invariants transport/runtime S1 à S5.

## 1. Baseline S6 validée

Baseline S5 de départ :

`9b4520f188c7d7da182bd9c08adb9e7032a96190`

`Supervision: freeze S5 pre-hardware operational runtime`

État S6-H validé par revue opérateur :

`603984e4afe2176c005619584c88f6478a82ab83`

`Supervision: add S6-H navigable web mockup`

La maquette se trouve dans :

`Modbus RTU/Supervision TR2/ux/s6-h/`

Elle est statique, pré-matériel, alimentée exclusivement par des fixtures non normatives et n'est pas intégrée à la solution .NET.

## 2. Objet et frontière de S6

S6 définit l'expérience opérateur de la future IHM Web locale de supervision TR2, accessible depuis plusieurs PC du navire.

S6 couvre :

- cadrage utilisateurs et principes UX ;
- navigation globale et contextuelle ;
- vue générale du parc TR2 ;
- détail TR2 et supervision vibratoire ;
- attention, diagnostic et maintenance ;
- commandes B5 et traitement de l'état `Ambiguous` ;
- campagnes B6 ;
- état système, communications et configuration PC en lecture seule ;
- maquette Web navigable de validation UX.

S6 ne choisit pas l'architecture Web de production et ne raccorde aucune Web API au runtime réel.

## 3. Tranches S6 gelées

```text
S6-A  cadrage IHM / utilisateurs / principes
S6-B  arborescence / navigation
S6-C  vue générale / parc TR2
S6-D  détail TR2 / vibrations
S6-E  attention / diagnostic / maintenance
S6-F  commandes B5 / Ambiguous
S6-G  campagnes / système / communications
S6-H  maquette Web navigable
S6-I  audit final et clôture documentaire
```

## 4. Principes UX gelés

Les profils suivants sont des personas UX et non des rôles logiciels :

1. exploitation ;
2. opération TR2 ;
3. maintenance / diagnostic.

Aucun login, ACL ou modèle de rôles n'est créé en S6.

Progression UX :

```text
Voir
→ Localiser le problème
→ Comprendre
→ Agir
→ Diagnostiquer en profondeur
```

Principes :

- métier avant protocole ;
- état système, état TR2 et communication restent distincts ;
- dernière valeur connue ne signifie jamais valeur actuelle ;
- fraîcheur PC et validité/fraîcheur TR2 restent distinctes ;
- aucune anomalie n'est indiquée uniquement par une couleur ;
- les actions conséquentes sont séparées de la consultation ;
- `Ambiguous` est un état de premier rang ;
- problèmes PC/communication distincts des faults/warnings TR2 ;
- progressive disclosure ;
- les fixtures UX ne créent aucune exigence firmware, registre ou API.

## 5. Navigation gelée

Navigation globale :

```text
Vue générale
Vibrations
Attention & diagnostic
Campagnes
Système
```

Les commandes ne constituent pas une navigation globale : elles sont toujours contextualisées sur un TR2 identifié.

Navigation contextuelle TR2 :

```text
Synthèse
Vibrations
État & configuration
Commandes
Campagnes
Diagnostic
```

L'IHM n'est jamais organisée autour des blocs B0 à B7. Les blocs restent des autorités de données/protocole et peuvent apparaître seulement dans le détail technique.

## 6. Vue générale du parc

La page d'accueil est le tableau du parc, pas un dashboard KPI séparé.

Le point de mesure et l'équipement précèdent l'identité technique du TR2.

Dimensions affichées séparément :

- communication ;
- fraîcheur des données PC ;
- vibration ;
- état TR2 ;
- attention UX ;
- dernière réception.

Aucun état métier global synthétique du TR2 n'est inventé.

`Attention` est une agrégation de présentation uniquement. Elle ne devient pas une autorité domaine ou protocolaire.

Un endpoint configuré non identifié reste visible. Un TR2 incompatible reste distinct d'un TR2 non joignable.

La dernière valeur connue reste affichable après perte de communication, mais elle est explicitement qualifiée comme non actuelle ; elle n'est jamais remplacée par zéro.

## 7. Vibrations V1

La vue vibration respecte strictement B3 V1 :

- toutes les grandeurs RMS et crête affichées sont des accélérations en `mg` ;
- RMS global et crête globale B3 sont les indicateurs principaux ;
- X/Y/Z sont complémentaires ;
- les valeurs globales fournies par B3 ne sont pas recalculées côté PC ;
- axe dominant, sévérité, dépassements, validité et métadonnées de calcul restent issus de B3 ;
- les seuils ne sont affichés que s'ils proviennent réellement de la configuration B4 active applicable ;
- fraîcheur B3/TR2 et fraîcheur de réception PC sont affichées séparément ;
- une valeur `LAST_VALUE_HELD`, dégradée ou invalide reste explicitement qualifiée.

S6 n'introduit pas :

- vitesse `mm/s` ;
- FFT ou spectre ;
- fréquence dominante ;
- conversion accélération/vitesse ;
- indice ISO 10816/20816 ;
- score santé ;
- moyenne artificielle X/Y/Z ;
- diagnostic vibratoire automatique ;
- seuil arbitraire.

L'historique des observations B3 côté supervision peut être représenté par des courbes, sans créer une nouvelle fonction analytique.

## 8. Attention, diagnostic et maintenance

La page globale est nommée `Attention & diagnostic` afin de ne pas assimiler toutes les situations à des alarmes TR2.

Familles conservées séparément :

```text
Vibration            source B3
État / défaut TR2    sources B1 et B7
Communication PC     supervision / transport / session
Opération            B5 et journal transactionnel PC
```

Aucune sévérité universelle n'est recalculée entre B1, B3, B7, communication et B5.

B1 reste la synthèse système TR2. B7 reste le diagnostic interne. Ils ne sont pas fusionnés en une nouvelle autorité.

La communication interne signalée par B7 est distinguée de la communication Modbus PC ↔ TR2.

Le dernier défaut B7 n'est pas présenté comme un historique complet.

Le mode maintenance est visible sur toutes les pages du TR2 lorsqu'il est réellement actif.

Une `ModbusExceptionResponse` valide n'est pas assimilée automatiquement à une rupture physique.

Il n'existe pas d'`Acquitter tout` et une erreur de communication PC n'est pas acquittable.

## 9. Commandes B5

L'IHM ne pilote jamais directement les registres B5. Elle sollicite le moteur transactionnel de supervision pour un `device_id` déterminé.

Le `transaction_id`, les clés de confirmation et les écritures Modbus sont invisibles à l'opérateur normal.

Une seule transaction non terminale est autorisée par `device_id` conformément aux invariants gelés.

Les états supervision `Prepared`, `Submitted` et `Ambiguous` restent distincts des états observés dans B5.

`Ambiguous` signifie résultat incertain, jamais échec.

Lorsqu'une transaction est ambiguë :

- les nouvelles commandes sont bloquées ;
- aucun replay automatique n'est proposé ;
- aucun bouton `Réessayer` n'est proposé ;
- aucun bouton `Ignorer`, `Forcer résolu` ou équivalent n'est proposé ;
- une reconnexion seule ne suffit pas à résoudre l'ambiguïté ;
- la résolution repose sur les preuves B5 prévues par le moteur de réconciliation ;
- l'ambiguïté persiste à travers redémarrage de la supervision tant qu'elle n'est pas résolue.

Les confirmations UX sont distinctes des mécanismes de confirmation B5.

`SOFTWARE_RESET` reçoit une confirmation UX renforcée ; la clé B5 reste interne.

L'IHM n'applique pas d'optimistic update pour acquisition, maintenance ou configuration : l'état réel doit être confirmé.

`SELFTEST` distingue résultat transactionnel B5 et résultat fonctionnel B7.

`ACKNOWLEDGE_FAULT` n'est pas présenté comme un acquittement global.

Aucun bouton générique d'annulation n'est exposé tant que la matrice d'annulabilité réellement applicable n'est pas spécifiée et raccordée.

`RESET_STATISTICS` reste absent de l'IHM avec le scope firmware courant volontairement vide.

## 10. Campagnes B6

B6 est présenté comme un inventaire de métadonnées de campagnes.

Identité globale :

```text
(device_id, campaign_id)
```

Les états, intégrité, compteurs et métadonnées restent ceux de B6.

`duration_s` est l'autorité d'affichage de durée ; l'IHM ne remplace pas systématiquement cette valeur par `end_timestamp - start_timestamp`.

`selected_campaign_index` reste un détail protocolaire invisible à l'utilisateur.

La navigation B6 n'est jamais transformée en transaction B5.

S6 ne crée aucun téléchargement Modbus des données brutes, import SD, parser, FFT ou analyse de campagne.

## 11. Système et communications

`Système` est séparé en :

```text
Supervision
Communications
Configuration PC
```

La readiness du runtime PC ne signifie pas que tous les TR2 sont joignables ou nominaux.

Le bus logique reste distinct du port COM. Le port COM n'est jamais une identité d'équipement.

Un endpoint configuré reste visible sans `device_id` identifié.

Les incidents de communication utilisent la taxonomie héritée :

```text
Unclassified
Timeout
Io
ModbusExceptionResponse
```

Aucun diagnostic physique comme polarité A/B, terminaison, bias, parasites ou panne d'adaptateur n'est déduit d'une erreur logicielle pré-matériel.

Après I/O et invalidation de session, la reconnexion exige une nouvelle identification B0 conformément aux invariants transport gelés.

La configuration PC est affichée en lecture seule.

S6 n'introduit ni éditeur Web de configuration, ni bouton de restart du host, ni hypothèse de Windows Service.

## 12. Maquette S6-H

La maquette validée est :

`Modbus RTU/Supervision TR2/ux/s6-h/index.html`

avec :

- `styles.css` ;
- `app.js` ;
- `README.md`.

Elle matérialise les parcours S6-A à S6-G avec des fixtures explicitement non normatives.

Elle ne possède :

- aucune Web API ;
- aucun accès Modbus ;
- aucune commande réelle ;
- aucun framework Web imposé ;
- aucune intégration à la solution .NET.

Elle constitue un prototype UX et non une baseline d'architecture Web de production.

## 13. Audit transversal S6-I

Audit de clôture effectué contre S0 à S5 et la spécification V1 pertinente.

Résultat : **aucun écart bloquant identifié nécessitant une modification du runtime ou de la spécification V1**.

Les invariants suivants restent préservés :

- navigateur sans accès Modbus direct ;
- moteur de supervision unique propriétaire du transport et de l'autorité B5 ;
- multi-TR2 ;
- identité durable par `device_id` ;
- perte de communication sans remise à zéro des dernières valeurs ;
- fraîcheur PC distincte de la validité métier TR2 ;
- B5 sans replay automatique ;
- recovery durable `Ambiguous` ;
- B6 sans fausse transaction B5 ;
- séparation problèmes PC / défauts TR2 ;
- absence d'import silencieux de règles V1.1 ;
- absence de conclusion matérielle depuis les doubles pré-matériel.

S6 ajoute des décisions de présentation et d'expérience opérateur. Elles relèvent de **SUPERVISION_POLICY / UX_POLICY**, sauf lorsqu'elles exposent directement une sémantique normative V1 existante.

S6 ne crée aucune nouvelle **FW_POLICY**.

Toute lacune V1 reste `NOT_DEFINED V1` tant qu'elle n'est pas arbitrée par l'autorité appropriée.

## 14. Validation obtenue

Les tranches S6-A à S6-G ont été revues et explicitement validées par l'utilisateur au fil de la conception.

La maquette S6-H a ensuite été intégrée sur `main` et revue visuellement par l'utilisateur, qui a confirmé qu'elle convenait.

Le commit S6-H n'affecte pas les projets .NET ni le runtime existant ; il ajoute uniquement des fichiers statiques sous `ux/s6-h/`.

Le présent commit est documentaire et constitue le gel S6.

## 15. Hors périmètre après S6

Restent notamment ouverts :

- choix de l'architecture Web de production ;
- Web API réelle ;
- raccordement de l'IHM aux services applicatifs ;
- politique de push/poll HTTP/WebSocket éventuelle ;
- authentification et rôles ;
- sécurité réseau et exposition multi-PC finale ;
- write path B4 Web éventuel ;
- import/parser SD ;
- stockage brut campagne ;
- FFT et analyses vibratoires ;
- rétention, backup/restore, export et chiffrement ;
- synchronisation analytique/Grafana ;
- packaging/installer ;
- Windows Service et compte de service ;
- qualification réelle USB/RS-485 ;
- validation physique STM32/TR2 ;
- paramètres série réels et provisioning.

## 16. Conclusion de gel

S6 est clôturée comme **baseline UX pré-matériel**.

La maquette S6-H est la référence visuelle de l'expérience opérateur validée, mais elle ne doit pas être interprétée comme une décision de framework, de Web API ou d'architecture de déploiement.

Toute implémentation Web réelle ultérieure devra préserver les invariants gelés dans ce document et dans S0 à S5, ou faire l'objet d'un nouvel arbitrage explicite.
