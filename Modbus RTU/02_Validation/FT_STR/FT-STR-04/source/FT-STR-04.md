\# FT-STR-04 — Fiche de spécification  

\## Encodage des chaînes ASCII fixes



---



\## 1. Identification



\- \*\*ID\*\* : FT-STR-04  

\- \*\*Nom\*\* : ASCII fixe  

\- \*\*Famille parente\*\* : FT-STR  

\- \*\*Criticité\*\* : P1 (majeur)



---



\## 2. Objectif



Valider que les chaînes ASCII :



\- respectent la longueur fixe,

\- sont correctement encodées,

\- sont paddées en `0x00`,

\- ne contaminent pas les champs adjacents.



---



\## 3. Périmètre



\### Inclus



\- longueur fixe

\- encodage ASCII

\- padding 0x00

\- absence de débordement

\- comportement chaîne vide / pleine



\### Exclus



\- sémantique des chaînes

\- caractères autorisés métier



---



\## 4. Références



\- ASCII fixe (padding 0x00)

\- taille définie dans mapping



---



\## 5. Règles de conformité



Une chaîne est conforme si :



\- sa longueur est fixe

\- les caractères sont ASCII valides

\- les octets inutilisés = 0x00

\- aucun débordement



---



\## 6. Préconditions



\- FT-STR-02 validée



---



\## 7. Résultats attendus



\- chaînes propres

\- padding correct

\- lecture stable



---



\## 8. Risques



| Risque | Impact |

|---|---|

| Mauvais padding | pollution |

| Mauvaise longueur | décalage |

| Caractères invalides | parsing cassé |

| Débordement | corruption |



---



\## 9. Cas de test



\### Nominal

\- chaîne partiellement remplie

\- chaîne pleine



\### Limites

\- chaîne vide

\- longueur max



\### Erreurs

\- absence de padding

\- caractères invalides



\### Robustesse

\- lecture répétée



---



\## 10. Points critiques



\- padding obligatoire

\- zéro terminal non garanti → vérifier toute la zone

\- résidus mémoire fréquents



---



\## 11. Ambiguïtés



\- ASCII strict ou étendu ?

\- caractères autorisés ?

\- trim côté centrale ?



---



\## 12. Critères de réussite



\- 100% ASCII conforme

\- padding correct

\- aucune contamination



---



\## 13. Échec



\- octets non nuls dans padding

\- débordement

\- encodage invalide



---



\## 14. Anomalies



| Type | Exemple |

|---|---|

| MAJEURE | mauvais padding |

| MINEURE | caractère non critique |

| SPEC | règle ASCII floue |



---



\## 15. Dépendances



\### Amont

\- FT-STR-02



---



\## 16. Ordre



1\. Lire chaîne

2\. Vérifier longueur

3\. Vérifier ASCII

4\. Vérifier padding

5\. Vérifier isolation



---



\## 17. Livrables



\- dump chaînes

\- validation padding



---



\## 18. Maturité



\- chaînes propres

\- padding strict

\- comportement stable



---

