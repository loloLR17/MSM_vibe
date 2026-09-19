# P12-H3c3-C2 — Recovery global du store borné

## 1. Objet

Définir les invariants du scan de recovery du futur `CommandJournalStore` borné avant son implémentation.

Le recovery parcourt les 256 slots logiques définis par C1 et utilise exclusivement la sélection A/B gelée en C1.

Cette tranche ne définit pas encore l'admission, l'éviction ni les mutations persistantes.

## 2. États élémentaires d'un slot

Pour chaque slot logique, le helper C1 fournit l'un des états :

- EMPTY ;
- VALID avec un `CommandJournalBoundedRecord` courant ;
- CORRUPTED ;
- UNSUPPORTED ;
- UNAVAILABLE.

Le recovery ne tente aucune réparation implicite d'un slot non VALID.

## 3. Règles globales de classification

Le scan porte sur les 256 slots.

- tous les slots EMPTY -> recovery EMPTY ;
- au moins un slot VALID et aucune anomalie globale -> recovery VALID ;
- tout slot UNAVAILABLE -> recovery UNAVAILABLE ;
- en l'absence de UNAVAILABLE, tout slot UNSUPPORTED -> recovery UNSUPPORTED ;
- en l'absence de UNAVAILABLE/UNSUPPORTED, tout slot CORRUPTED -> recovery CORRUPTED.

Les anomalies structurelles décrites ci-dessous donnent également CORRUPTED.

Le recovery est fail-closed : tant qu'il n'aboutit pas à EMPTY ou VALID, `recovery_required` reste vrai.

## 4. Unicité des transaction_id

Chaque record VALID doit contenir un `transaction_id` métier cohérent, déjà contrôlé par le codec V3.

Deux slots logiques VALID portant le même `transaction_id` constituent une corruption structurelle, même si leur contenu est identique.

Le recovery retourne CORRUPTED. Il ne choisit jamais arbitrairement l'un des deux slots.

## 5. Transaction non terminale

Les états RESERVED et STARTED sont non terminaux.

Le store peut contenir au maximum une transaction non terminale à l'issue du scan.

Deux slots VALID ou davantage contenant des transactions non terminales constituent une corruption structurelle et donnent CORRUPTED.

Le recovery ne transforme ni RESERVED ni STARTED ; la résolution métier de l'unique transaction incomplète reste du ressort de `command_boot_recovery`.

## 6. admission_order

Chaque record VALID possède un `admission_order != 0`.

Les `admission_order` des entrées courantes doivent être uniques dans la fenêtre persistante. Un doublon entre deux slots VALID donne CORRUPTED : l'ordre d'admission et donc l'ordre d'éviction seraient ambigus.

Le recovery calcule :

```text
max_admission_order = max(admission_order des slots VALID)
```

Si aucun record n'existe :

```text
next_admission_order = 1
```

Sinon :

```text
next_admission_order = max_admission_order + 1
```

Si `max_admission_order == UINT32_MAX`, aucun wrap n'est autorisé. Le recovery qualifie l'image UNSUPPORTED : le format/compteur courant ne permet plus une admission sûre sans politique supplémentaire.

Aucune renumérotation automatique n'est effectuée pendant le recovery.

## 7. completion_order

Pour les records COMPLETED, `completion_order` est non nul ; pour RESERVED/STARTED il vaut zéro, conformément à la cohérence du record V3.

Les `completion_order` non nuls des entrées courantes doivent être uniques. Un doublon entre deux transactions COMPLETED donne CORRUPTED car `latest_completed` deviendrait ambigu.

Le recovery calcule :

```text
max_completion_order = max(completion_order des COMPLETED)
```

Si aucun COMPLETED n'existe :

```text
next_completion_order = 1
```

Sinon :

```text
next_completion_order = max_completion_order + 1
```

Si `max_completion_order == UINT32_MAX`, aucun wrap n'est autorisé. Le recovery qualifie l'image UNSUPPORTED.

Aucune renumérotation automatique n'est effectuée.

## 8. generation

La génération est locale à un slot physique et sert uniquement à la sélection A/B.

Le recovery global ne compare pas les générations entre slots.

Un record sélectionné avec `generation == UINT32_MAX` reste lisible et peut participer au recovery VALID. Le slot est cependant non mutable par les tranches ultérieures ; aucune réécriture ni remise à zéro de génération n'est autorisée.

La présence d'un tel slot ne rend donc pas à elle seule l'image globale invalide.

## 9. Compteurs reconstruits

Après un recovery EMPTY ou VALID réussi, le store mémorise au minimum :

- `next_admission_order` ;
- `next_completion_order` ;
- le nombre de transactions connues.

Le futur résultat de recovery devra exposer les compteurs nécessaires aux tests et au diagnostic, sans exposer les détails physiques A/B.

## 10. Atomicité du recovery

Le scan ne modifie aucun octet du média.

Les champs runtime reconstruits ne deviennent actifs qu'après réussite complète du scan.

En cas de CORRUPTED, UNSUPPORTED ou UNAVAILABLE :

- `recovery_required` reste vrai ;
- aucun état partiellement reconstruit ne doit rendre le journal utilisable ;
- aucune tentative de réparation ou d'éviction n'est effectuée.

## 11. Cas de test minimaux C2

L'implémentation C2 devra couvrir au minimum :

- 256 slots vides ;
- un record valide ;
- transaction_id 1 et 65535 dans des slots arbitraires ;
- plusieurs records COMPLETED avec reconstruction des deux compteurs ;
- un unique RESERVED ;
- un unique STARTED ;
- deux non-terminaux -> CORRUPTED ;
- transaction_id dupliqué -> CORRUPTED ;
- admission_order dupliqué -> CORRUPTED ;
- completion_order dupliqué -> CORRUPTED ;
- slot CORRUPTED ;
- slot UNSUPPORTED ;
- slot UNAVAILABLE ;
- max admission_order UINT32_MAX -> UNSUPPORTED ;
- max completion_order UINT32_MAX -> UNSUPPORTED ;
- génération UINT32_MAX sélectionnée -> recovery possible, slot ultérieurement non mutable.

## 12. Hors périmètre

Restent hors C2 :

- `find`, `visit`, `latest_completed` du store borné ;
- admission d'une nouvelle transaction ;
- choix d'un slot libre ;
- éviction du plus ancien COMPLETED ;
- écriture A/B et mutation des entrées ;
- politique de traitement d'un slot dont la génération est épuisée lors d'une future admission ;
- choix du média persistant STM32 de production.
